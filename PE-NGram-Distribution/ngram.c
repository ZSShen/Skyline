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
    uchar       ucIdxBitVF, ucIdxBitVT, ucIdxBitCur, ucBitVal, ucMask;
    ushort      usNumRegions, usShiftPos, usIdxSection;
    ulong       ulSecRawSize, ulSecRawOffset, ulIdxBgn, ulIdxEnd, ulOstBgn, ulOstEnd, ulRegionSize;
    ulong       ulIdxByteRF, ulIdxByteVF, ulIdxByteVT, ulIdxByteCur, ulTokenVal;
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
                ulRegionSize = ulOstEnd - ulOstBgn;
                
                //----------------------------------------------------
                // Main algorithm of the n-gram token collection.
                //----------------------------------------------------
                /* Preload a chunk of binary. */
                Fseek(pPEInfo->fpSample, ulOstBgn, SEEK_SET);
                nExptRead = (ulRegionSize < BUF_SIZE_LARGE)? ulRegionSize : BUF_SIZE_MID;
                nRealRead = Fread(buf, sizeof(uchar), nExptRead, pPEInfo->fpSample);

                ulIdxByteRF = _ulDimension - 1;
                ulIdxByteVF = ulIdxByteRF;
                ulIdxByteVT = 0;
                ucIdxBitVF = 0;
                ucIdxBitVT = BIT_MOST_SIGNIFICANT;
                while (TRUE) {
                    ucIdxBitCurr = ucIdxBitVT;
                    ulIdxByteCurr = ulIdxByteVT;

                    /* Generate token using the binary within the sliding window. */
                    ulTokenVal = 0;
                    for (k = 0 ; k < usShiftPos ; k++) {
                        ucMask = 1;
                        ucMask <<= ucIdxBitCur;
                        ucBitVal = buf[ulIdxByteCur] & ucMask;
                        ucBitVal >>= ucIdxBitCur;
                        ulTokenVal = (ulTokenVal << 1) + ucBitVal; 
                    
                        ucIdxBitCur--;
                        if (ucIdxBitCur < 0) {
                            ucIdxBitCur = BIT_MOST_SIGNIFICANT;
                            ulIdxByteCur++;
                            if (ulIdxByteCur == BUF_SIZE_LARGE)
                                ulIdxByteCur = 0;
                        }
                    }

                    if (self->arrToken[ulTokenVal] == NULL) {
                        self->arrToken[ulTokenVal] = (Token*)Malloc(sizeof(Token));
                        self->arrToken[ulTokenVal]->ulValue = ulTokenVal;
                        self->arrToken[ulTokenVal]->ulFrequency = 0;                    
                    }
                    self->arrToken[ulTokenVal]->ulFrequency++;
                    
                    /* Adjust the front-end location pointers. */
                    ucIdxBitVF--;
                    if(ucIdxBitVF < 0) {
                        ucIdxBitVF = BIT_MOST_SIGNIFICANT;
                        
                        /* Check the condition to terminate the algorithm. */
                        ulIdxByteRF++;
                        if(ulIdxByteRF == ulRegionSize)
                            break;

                        ulIdxByteVF++;
                        if(ulIdxByteVF == BUF_SIZE_LARGE) {
                            ulIdxByteVF = 0;
                            
                            /* Put a new chunk of data to the buffer with offset 
                               from 0 to (BUF_SIZE_LARGE - _usDimension - 1). */
                            nExptRead = BUF_SIZE_LARGE - _usDimension;
                            nRealRead = Fread(buf, sizeof(uchar), nExptRead, pPEInfo->fpSample);
                        }        
                    }
                    
                    /* Adjust the tail-end location pointer. */
                    ucIdxBitVT--;
                    if(ucIdxBitVT < 0) {
                        ucIdxBitVT = BIT_MOST_SIGNIFICANT;
                        
                        ulIdxByteVT++;
                        if(ulIdxByteVT == BUF_SIZE_LARGE) {
                            ulIdxByteVT = 0;
                            
                            /* Put a new chunk of data to the buffer with offset
                               from (BUF_SIZE_LARGE - _usDimension) to (BUF_SIZE_LARGE - 1). */
                            nExptRead = _usDimension;
                            nRealRead = Fread(buf + BUF_SIZE_LARGE - _usDimension, sizeof(uchar), nExptRead, pPEInfo->fpFile);
                        }
                    }
                }

            }

            /*
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
