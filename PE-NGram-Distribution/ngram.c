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


/**
 * This function hints the qsort() library to sort the n-gram tokens by their appearance frequency.
 *
 * @param   pSrc        The pointer to the source token.
 * @param   pDst        The pointer to the target token.
 *
 * @return             < 0: The source token must go before the target one.
 *                       0: The source and target tokens do not need to change their order.
 *                     > 0: The source token must go after the target one. 
 */
int _CompTokenFreqDescOrder(const void *pSrc, const void *pTge);


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
        for (i = 0 ; i < _ulMaxValue ; i++) {
            pToken = self->arrToken[i];
            if (pToken != NULL) { 
                Free(pToken);
            }
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
                ulOstBgn = ulSecRawOffset + ulIdxBgn * ENTROPY_BLK_SIZE;
                ulOstEnd = ulSecRawOffset + ulIdxEnd * ENTROPY_BLK_SIZE;
                ulRegionSize = ulOstEnd - ulOstBgn;

                //----------------------------------------------------
                // Main algorithm for the n-gram token collection.
                //----------------------------------------------------
                /* Preload a chunk of binary. */
                Fseek(pPEInfo->fpSample, ulOstBgn, SEEK_SET);
                nExptRead = (ulRegionSize < BUF_SIZE_LARGE)? ulRegionSize : BUF_SIZE_MID;
                memset(buf, 0, sizeof(uchar) * nExptRead);
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

                    if (self->arrToken[ulTokenVal] == NULL) {
                        self->ulNumTokens++;
                        self->arrToken[ulTokenVal] = (Token*)Malloc(sizeof(Token));
                        self->arrToken[ulTokenVal]->ulValue = ulTokenVal;
                        self->arrToken[ulTokenVal]->ulFrequency = 0;   
                    }
                    self->arrToken[ulTokenVal]->ulFrequency++;

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
                               from 0 to (BUF_SIZE_LARGE - _ucDimension - 1). */
                            nExptRead = BUF_SIZE_LARGE - _ucDimension;
                            if (nExptRead > (ulRegionSize - ulIdxByteRF))
                                nExptRead = ulRegionSize - ulIdxByteRF;
                            memset(buf, 0, sizeof(uchar) * nExptRead);
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
                               from (BUF_SIZE_LARGE - _ucDimension) to (BUF_SIZE_LARGE - 1). */
                            nExptRead = _ucDimension;
                            if (nExptRead > (ulRegionSize - ulIdxByteRF))
                                nExptRead = ulRegionSize - ulIdxByteRF;
                            memset(buf, 0, sizeof(uchar) * nExptRead);
                            nRealRead = Fread(buf + BUF_SIZE_LARGE - _ucDimension, sizeof(uchar), nExptRead, pPEInfo->fpSample);
                        }
                    }
                }
                /* End of one binary region. */
            }
            /* End of one section. */
        }
 
        /* Sort the tokens. */
        //qsort(self->arrToken, _ulMaxValue, sizeof(Token*), _CompTokenFreqDescOrder);
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


int _CompTokenFreqDescOrder(const void *pSrc, const void *pTge) {
    
    if (pSrc == NULL) {
        if (pTge == NULL)
            return 0;
        else
            return 1;
    } else {
        if (pTge == NULL)
            return -1;
        else {
            return -(((Token*)pSrc)->ulFrequency - ((Token*)pTge)->ulFrequency);
        }
    }
}
