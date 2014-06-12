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
 * This function generates the model by sorting the appearance frequencies of n-gram tokens
 * with descending order.
 *
 * @param   self                The pointer to the NGram structure.
 * @param   pPEInfo             The pointer to the to be analyzed PEInfo structure. 
 * @param   pRegionCollector    The pointer to the RegionCollector structure which stores all the selected features.
 *
 * @return                      0: The model is generated successfully.
 *                            < 0: Exception occurs while memory allocation.
 */
int _FuncTokenFreqDescOrder(NGram *self, PEInfo *pPEInfo, RegionCollector *pRegionCollector);


/*===========================================================================*
 *                Implementation for exported functions                      *
 *===========================================================================*/

void NGramInit(NGram *self) {

    /* Initialize member variables. */
    _ucDimension = 0;
    _ulMaxValue = 0;
    self->ulNumTokens = 0;
    self->arrToken = NULL;

    /* Assign the default member functions */
    self->setDimension = NGramSetDimension;
    self->generateModel = NGramGenerateModel;    

    return;
}


void NGramDeinit(NGram *self) {
    int     i;
    Token   *pToken;

    if (self->arrToken != NULL) {
        /* Free the array of Token structures. */
        for (i = 0 ; i < self->ulNumTokens ; i++) {
            pToken = self->arrToken[i];
            if (pToken != NULL)
                Free(pToken);
        }
        Free(self->arrToken);
    }

    return;
}


/**
 * NGramSetDimension(): Set the maximum value of the n-gram token with the specified dimension.
 */
void NGramSetDimension(NGram *self, uchar ucDimension) {

    _ucDimension = ucDimension;
    _ulMaxValue = pow(UNI_GRAM_MAX_VALUE, ucDimension);
    return;
}


/**
 * NGramGenerateModel(): Generate the n-gram model based on the specified method.
 */
int NGramGenerateModel(NGram *self, const char *cszMethod, PEInfo *pPEInfo, RegionCollector *pRegionCollector) {

    if (cszMethod == NULL)
        _FuncTokenFreqDescOrder(self, pPEInfo, pRegionCollector);

    return 0;
}


/*===========================================================================*
 *                Implementation for internal functions                      *
 *===========================================================================*/

int _FuncTokenFreqDescOrder(NGram *self, PEInfo *pPEInfo, RegionCollector *pRegionCollector) {
    int         rc, i, j;
    uchar       ucIdxBitVF, ucIdxBitVT;
    ushort      usNumRegions, usShiftPos, usIdxSection;
    ulong       ulSecRawSize, ulSecRawOffset, ulIdxBgn, ulIdxEnd, ulOstBgn, ulOstEnd;
    ulong       ulIdxByteRF, ulIdxByteVF, ulIdxByteVT;
    size_t      nExptRead, nRealRead;
    Region      *pRegion;
    RangePair   **arrRangePair;
    uchar       buf[BUF_SIZE_LARGE];

    rc = 0;
    try {
        //----------------------------------------------------
        // First part: Collect and sort the n-gram tokens.
        //----------------------------------------------------

        usNumRegions = pRegionCollector->usNumRegions;
        if (usNumRegions == 0)
            goto EXIT;

        if (_ucDimension == 0)
            goto EXIT;

        self->arrToken = (Token**)Malloc(sizeof(Token*) * _ulMaxValue);
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
                ulOstBgn = ulSecRawSize + ulIdxBgn * ENTROPY_BLK_SIZE;
                ulOstEnd = ulSecRawSize + ulIdxEnd * ENTROPY_BLK_SIZE;
                
                //----------------------------------------------------
                // Main algorithm of the n-gram token collection.
                //----------------------------------------------------
                /* Preload a chunk of binary. */
                Fseek(pPEInfo->fpSample, ulOstBgn, SEEK_SET);
                nExptRead = ((ulOstEnd - ulOstBgn) < BUF_SIZE_LARGE)? (ulOstEnd - ulOstBgn) : BUF_SIZE_MID;
                nRealRead = Fread(buf, sizeof(uchar), nExptRead, pPEInfo->fpSample);

                ulIdxByteRF = _ulDimension - 1;
                ulIdxByteVF = ulIdxByteRF;
                ulIdxByteVT = 0;
                ucIdxBitVF = 0;
                ucIdxBitVT = BIT_MOST_SIGNIFICANT;
                while (TRUE) {
                    idxByteCur = idxByteVT;
                    idxBitCur = idxBitVT;
                    
                    // Calculate n-gram using the binary within the sliding window.
                    ulNGramVal = 0;
                    for (k = 0 ; k < iSftCount ; k++) {
                        ucMask = 1;
                        ucMask <<= idxBitCur;
                        ucBit = buf[idxByteCur] & ucMask;
                        ucBit >>= idxBitCur;
                        ulNGramVal = (ulNGramVal << 1) + ucBit; 
                    
                        idxBitCur--;
                        if (idxBitCur < 0) {
                            idxBitCur = BIT_MOST_SIGNIFICANT;
                            idxByteCur++;
                            if (idxByteCur == BUF_SIZE_LARGE)
                                idxByteCur = 0;
                        }
                    }
                    refSet.arrToken[ulNGramVal].ulValue = ulNGramVal;
                    refSet.arrToken[ulNGramVal].ulFrequency++;
                    
                    // Adjust the front-end location pointers.
                    idxBitVF--;
                    if(idxBitVF < 0) {
                        idxBitVF = BIT_MOST_SIGNIFICANT;
                        
                        // Check for the condition to terminate the algorithm.
                        ulIdxByteRF++;
                        if(ulIdxByteRF == ulBlkSize)
                            break;

                        idxByteVF++;
                        if(idxByteVF == BUF_SIZE_LARGE) {
                            idxByteVF = 0;
                            
                            // Circularly fill the buffer with new chunk of data.
                            iExptRead = BUF_SIZE_LARGE - iNGramDimension;
                            iRealRead = Fread(buf, sizeof(uchar), iExptRead, pPEInfo->fpFile);
                        }        
                    }
                    
                    // Adjust the tail-end location pointer.
                    idxBitVT--;
                    if(idxBitVT < 0) {
                        idxBitVT = BIT_MOST_SIGNIFICANT;
                        
                        idxByteVT++;
                        if(idxByteVT == BUF_SIZE_LARGE) {
                            idxByteVT = 0;
                            
                            // Circularly fill the buffer with new chunk of data.
                            iExptRead = iNGramDimension;
                            iRealRead = Fread(buf + BUF_SIZE_LARGE - iNGramDimension, sizeof(uchar), iExptRead, pPEInfo->fpFile);
                        }
                    }
                }

            }

            /*
            listRangePair = refRegion.listRangePair;
            while (listRangePair != NULL) {
                
                // Start the n-gram calculation.
                ulIdxByteRF = iNGramDimension - 1;
                idxByteVF = ulIdxByteRF;
                idxByteVT = 0;
                idxBitVF = 0;
                idxBitVT = BIT_MOST_SIGNIFICANT;
                while (TRUE) {
                    idxByteCur = idxByteVT;
                    idxBitCur = idxBitVT;
                    
                    // Calculate n-gram using the binary within the sliding window.
                    ulNGramVal = 0;
                    for (k = 0 ; k < iSftCount ; k++) {
                        ucMask = 1;
                        ucMask <<= idxBitCur;
                        ucBit = buf[idxByteCur] & ucMask;
                        ucBit >>= idxBitCur;
                        ulNGramVal = (ulNGramVal << 1) + ucBit; 
                    
                        idxBitCur--;
                        if (idxBitCur < 0) {
                            idxBitCur = BIT_MOST_SIGNIFICANT;
                            idxByteCur++;
                            if (idxByteCur == BUF_SIZE_LARGE)
                                idxByteCur = 0;
                        }
                    }
                    refSet.arrToken[ulNGramVal].ulValue = ulNGramVal;
                    refSet.arrToken[ulNGramVal].ulFrequency++;
                    
                    // Adjust the front-end location pointers.
                    idxBitVF--;
                    if(idxBitVF < 0) {
                        idxBitVF = BIT_MOST_SIGNIFICANT;
                        
                        // Check for the condition to terminate the algorithm.
                        ulIdxByteRF++;
                        if(ulIdxByteRF == ulBlkSize)
                            break;

                        idxByteVF++;
                        if(idxByteVF == BUF_SIZE_LARGE) {
                            idxByteVF = 0;
                            
                            // Circularly fill the buffer with new chunk of data.
                            iExptRead = BUF_SIZE_LARGE - iNGramDimension;
                            iRealRead = Fread(buf, sizeof(uchar), iExptRead, pPEInfo->fpFile);
                        }        
                    }
                    
                    // Adjust the tail-end location pointer.
                    idxBitVT--;
                    if(idxBitVT < 0) {
                        idxBitVT = BIT_MOST_SIGNIFICANT;
                        
                        idxByteVT++;
                        if(idxByteVT == BUF_SIZE_LARGE) {
                            idxByteVT = 0;
                            
                            // Circularly fill the buffer with new chunk of data.
                            iExptRead = iNGramDimension;
                            iRealRead = Fread(buf + BUF_SIZE_LARGE - iNGramDimension, sizeof(uchar), iExptRead, pPEInfo->fpFile);
                        }
                    }
                }
            }
            // Sort the n-gram tokens by descending order based on their appearing frequency. 
            qsort(refSet.arrToken, ulNGramSpace, sizeof(NGramToken), FuncCompareTokenDescOrder);
            pModel->arrTokenSet[i] = refSet;
            */
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
