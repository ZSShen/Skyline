#include "ngram.h"


/*===========================================================================*
 *                  Simulation for private variables                         *
 *===========================================================================*/
/* The maximum value of the n-gram token with the specified dimension. */
uchar _ucDimension;
ulong _ulMaxValue;


/*===========================================================================*
 *                  Definition for internal functions                        *
 *===========================================================================*/
/**
 * This function collects the n-gram tokens from the specified binary regions.
 *
 * @param   self                The pointer to the NGram structure.
 * @param   pPEInfo             The pointer to the to be analyzed PEInfo structure.
 * @param   pRegionCollector    The pointer to the RegionCollector structure which stores all the selected features.
 *
 * @return                      0: The tokens are collected successfully.
 *                            < 0: Exception occurs while memory allocation.
 */
int _NGramCollectTokens(NGram *self, PEInfo *pPEInfo, RegionCollector *pRegionCollector);


/*===========================================================================*
 *                Implementation for exported functions                      *
 *===========================================================================*/
void NGramInit(NGram *self) {
    /* Initialize member variables. */
    _ucDimension = 0;
    _ulMaxValue = 0;
    self->ulNumTokens = 0;
    self->ulNumSlices = 0;
    self->arrToken = NULL;
    self->arrSlice = NULL;
    self->hdlePlug = NULL;
    self->entryPlug = NULL;

    /* Assign the default member functions */
    self->loadPlugin = NGramLoadPlugin;
    self->unloadPlugin = NGramUnloadPlugin;
    self->setDimension = NGramSetDimension;
    self->generateModel = NGramGenerateModel;
    self->dump = NGramDump;

    return;
}

void NGramDeinit(NGram *self) {
    int     i;
    Token   *pToken;
    Slice   *pSlice;

    if (self->arrToken != NULL) {
        /* Free the array of Token structures. */
        for (i = 0 ; i < self->ulNumTokens ; i++) {
            pToken = self->arrToken[i];
            if (pToken != NULL) {
                Free(pToken);
            }
        }
        Free(self->arrToken);
    }

    if (self->arrSlice != NULL) {
        /* Free the array of Slice structures. */
        for (i = 0 ; i < self->ulNumSlices ; i++) {
            pSlice = self->arrSlice[i];
            if (pSlice != NULL) {
                Free(pSlice);
            }
        }
        Free(self->arrSlice);
    }

    return;
}

int NGramLoadPlugin(NGram *self, const char *cszName) {
    int rc;
    char szLib[BUF_SIZE_SMALL];

    rc = 0;
    try {
        memset(szLib, 0, sizeof(char) * BUF_SIZE_SMALL);
        if (cszName == NULL)
            sprintf(szLib, "../../plugin/release/lib%s.so", LIB_DEFAULT_DESC_FREQ);
        else
            sprintf(szLib, "lib%s.so", cszName);

        self->hdlePlug = Dlopen(szLib, RTLD_LAZY);
        self->entryPlug = Dlsym(self->hdlePlug, PLUGIN_ENTRY_MODEL);
    } catch(EXCEPT_DL_LOAD) {
        rc = -1;
    } catch(EXCEPT_DL_GET_SYMBOL) {
        rc = -1;
    } end_try;

    return rc;
}

int NGramUnloadPlugin(NGram *self) {
    if (self->hdlePlug != NULL)
        Dlclose(self->hdlePlug);
    return 0;
}

void NGramSetDimension(NGram *self, uchar ucDimension) {
    _ucDimension = ucDimension;
    _ulMaxValue = pow(UNI_GRAM_MAX_VALUE, ucDimension);
    return;
}

int NGramGenerateModel(NGram *self, PEInfo *pPEInfo, RegionCollector *pRegionCollector) {
    /* First, collect tokens from the specified binary regions. */
    int rc = _NGramCollectTokens(self, pPEInfo, pRegionCollector);
    if (rc != 0)
        return rc;

    /* Second, generate model using the specified method. */
    return self->entryPlug(self, _ulMaxValue);
}

void NGramDump(NGram *self) {
    int     i;
    Token   *pToken;

    /* Dump the n-gram tokens. */
    for (i = 0 ; i < self->ulNumTokens ; i++) {
        pToken = self->arrToken[i];
        if (pToken != NULL)
            printf("%d\t%04lx\t%lu\n", i, pToken->ulValue, pToken->ulFrequency);
    }

    return;
}


/*===========================================================================*
 *                Implementation for internal functions                      *
 *===========================================================================*/
int _NGramCollectTokens(NGram *self, PEInfo *pPEInfo, RegionCollector *pRegionCollector) {
    int         rc, i, j, k;
    char        cIdxBitVF, cIdxBitVT, cIdxBitCur;
    uchar       ucBitVal, ucMask;
    ushort      usNumRegions, usShiftPos, usIdxSection;
    ulong       ulSecRawSize, ulSecRawOffset, ulIdxBgn, ulIdxEnd, ulOstBgn, ulOstEnd, ulRegionSize;
    ulong       ulIdxByteRF, ulIdxByteVF, ulIdxByteVT, ulIdxByteCur, ulTokenVal;
    size_t      nExptRead, nRealRead;
    Region      *pRegion;
    RangePair   **arrRangePair;
    uchar       buf[BUF_SIZE_LARGE];

    rc = 0;
    try {
        usNumRegions = pRegionCollector->usNumRegions;
        if (usNumRegions == 0)
            goto EXIT;

        if (_ucDimension == 0)
            goto EXIT;

        self->arrToken = (Token**)Calloc(_ulMaxValue, sizeof(Token*));
        for (i = 0 ; i < _ulMaxValue ; i++)
            self->arrToken[i] = NULL;

        usShiftPos = _ucDimension * SHIFT_RANGE_8BIT;

        for (i = 0 ; i < usNumRegions ; i++) {
            /* Collect the n-gram tokens block by block within the current region(section). */
            pRegion = pRegionCollector->arrRegion[i];
            usIdxSection = pRegion->usIdxSection;
            ulSecRawSize = pPEInfo->arrSectionInfo[usIdxSection]->ulRawSize;
            ulSecRawOffset = pPEInfo->arrSectionInfo[usIdxSection]->ulRawOffset;

            arrRangePair = pRegion->arrRangePair;
            for (j = 0 ; j < pRegion->ulNumPairs ; j++) {
                ulIdxBgn = arrRangePair[j]->ulIdxBgn;
                ulIdxEnd = arrRangePair[j]->ulIdxEnd;

                /* Transform the data block index to the raw binary offset. */
                ulOstBgn = ulSecRawOffset + ulIdxBgn * ENTROPY_BLK_SIZE;
                ulOstEnd = ulSecRawOffset + ulIdxEnd * ENTROPY_BLK_SIZE;
                ulRegionSize = ulOstEnd - ulOstBgn;

                /*---------------------------------------------------*
                 * Main algorithm for the n-gram token collection.   *
                 *---------------------------------------------------*/
                /* Preload a chunk of binary. */
                Fseek(pPEInfo->fpSample, ulOstBgn, SEEK_SET);
                nExptRead = (ulRegionSize < BUF_SIZE_LARGE)? ulRegionSize : BUF_SIZE_LARGE;
                nRealRead = Fread(buf, sizeof(uchar), nExptRead, pPEInfo->fpSample);

                ulIdxByteRF = _ucDimension - 1;
                ulIdxByteVF = ulIdxByteRF;
                ulIdxByteVT = 0;
                cIdxBitVF = 0;
                cIdxBitVT = BIT_MOST_SIGNIFICANT;
                while (true) {
                    /* Generate token using the binary within the sliding window. */
                    cIdxBitCur = cIdxBitVT;
                    ulIdxByteCur = ulIdxByteVT;
                    ulTokenVal = 0;
                    for (k = 0 ; k < usShiftPos ; k++) {
                        ucMask = 1;
                        ucMask <<= cIdxBitCur;
                        ucBitVal = buf[ulIdxByteCur] & ucMask;
                        ucBitVal >>= cIdxBitCur;
                        ulTokenVal = (ulTokenVal << 1) + ucBitVal;

                        cIdxBitCur--;
                        if (cIdxBitCur < 0) {
                            cIdxBitCur = BIT_MOST_SIGNIFICANT;
                            ulIdxByteCur++;
                            if (ulIdxByteCur == BUF_SIZE_LARGE)
                                ulIdxByteCur = 0;
                        }
                    }

                    /* Ignore the dummy tokens: (ff)+ and (00)+. */
                    if ((ulTokenVal != 0) && (ulTokenVal != (_ulMaxValue - 1))) {
                        if (self->arrToken[ulTokenVal] == NULL) {
                            self->arrToken[ulTokenVal] = (Token*)Malloc(sizeof(Token));
                            self->arrToken[ulTokenVal]->ulValue = ulTokenVal;
                            self->arrToken[ulTokenVal]->ulFrequency = 0;
                            self->ulNumTokens++;
                        }
                        self->arrToken[ulTokenVal]->ulFrequency++;
                    }

                    /* Adjust the front-end location pointers. */
                    cIdxBitVF--;
                    if(cIdxBitVF < 0) {
                        cIdxBitVF = BIT_MOST_SIGNIFICANT;

                        /* Check the condition to terminate the algorithm. */
                        ulIdxByteRF++;
                        if(ulIdxByteRF == ulRegionSize)
                            break;

                        ulIdxByteVF++;
                        if(ulIdxByteVF == BUF_SIZE_LARGE) {
                            ulIdxByteVF = 0;

                            /* Put a new chunk of data to the buffer with offset
                               from 0 to (nExptRead - 1). */
                            nExptRead = BUF_SIZE_LARGE - _ucDimension;
                            if (nExptRead > (ulRegionSize - ulIdxByteRF))
                                nExptRead = ulRegionSize - ulIdxByteRF;
                            nRealRead = Fread(buf, sizeof(uchar), nExptRead, pPEInfo->fpSample);
                        }
                    }

                    /* Adjust the tail-end location pointer. */
                    cIdxBitVT--;
                    if(cIdxBitVT < 0) {
                        cIdxBitVT = BIT_MOST_SIGNIFICANT;

                        ulIdxByteVT++;
                        if(ulIdxByteVT == BUF_SIZE_LARGE) {
                            ulIdxByteVT = 0;

                            /* Put a new chunk of data to the buffer with offset
                               from (BUF_SIZE_LARGE - _ucDimension) to (BUF_SIZE_LARGE - 1 - _ucDimension + nExptRead). */
                            nExptRead = _ucDimension;
                            if (nExptRead > (ulRegionSize - ulIdxByteRF))
                                nExptRead = ulRegionSize - ulIdxByteRF;
                            nRealRead = Fread(buf + BUF_SIZE_LARGE - _ucDimension, sizeof(uchar), nExptRead, pPEInfo->fpSample);
                        }
                    }
                }
                /* End of one binary region. */
            }
            /* End of one section. */
        }
    } catch(EXCEPT_MEM_ALLOC) {
        rc = -1;
    } catch(EXCEPT_IO_FILE_READ) {
        rc = -1;
    } catch(EXCEPT_IO_FILE_SEEK) {
        rc = -1;
    } end_try;

EXIT:
    return rc;
}
