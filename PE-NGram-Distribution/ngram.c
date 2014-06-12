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
    int     rc, i;
    ushort  usNumRegions, usShiftPos;
    Region  *pRegion;

    int             idxByteVF, idxByteVT, idxBitVF, idxBitVT, idxByteCur, idxBitCur, iSftCount;
    uchar           ucMask, ucBit;
    ulong           ulSecRawSize, ulSecRawOffset, ulOffsetBgn, ulOffsetEnd, ulBlkSize;
    ulong           ulNGramSpace, ulIdxByteRF, ulNGramVal;
    Region          refRegion;
    NGramTokenSet   refSet;
    NGramModel      *pModel;
    RangePair       *listRangePair;
    uchar           buf[BUF_SIZE_LARGE];
    
    rc = 0;
    
    try {
        usNumRegions = pRegionCollector->usNumRegions;
        if (usNumRegions == 0)
            goto EXIT;

        if (_ucDimension == 0)
            goto EXIT;

        self->arrToken = (Token**)Malloc(sizeof(Token*) * _ulMaxValue);
        for (i = 0 ; i < _ulMaxValue ; i++)
            self->arrToken[i] = NULL;

        usShiftPos = _ucDimension * SHIFT_RANGE_8BIT;

        /*
        for (i = 0 ; i < usNumRegions ; i++) {
            refRegion = pPolicyRegion->arrRegion[i];
            refSet = pModel->arrTokenSet[i];
            
            // Collect n-gram tokens block by block within the designated section.
            idxSection = refRegion.idxSection;
            ulSecRawSize = pPEInfo->arrSectionInfo[idxSection]->ulRawSize;
            ulSecRawOffset = pPEInfo->arrSectionInfo[idxSection]->ulRawOffset;
            
            listRangePair = refRegion.listRangePair;
            while (listRangePair != NULL) {
                idxBgn = listRangePair->idxBgn;
                idxEnd = listRangePair->idxEnd;
                listRangePair = listRangePair->next;

                printf("0x%08x 0x%08x\n", idxBgn, idxEnd);
                
                // Determine the range of binary which should be calculated.
                ulOffsetBgn = ulSecRawOffset + idxBgn * ENTROPY_BLK_SIZE;
                ulOffsetEnd = ulSecRawOffset + idxEnd * ENTROPY_BLK_SIZE;

                // Move to the starting offset of this binary block.
                Fseek(pPEInfo->fpFile, ulOffsetBgn, SEEK_SET);
                
                // Pre-load a full line of binary.
                ulBlkSize = ulOffsetEnd - ulOffsetBgn;
                iExptRead = (ulBlkSize > BUF_SIZE_LARGE)? BUF_SIZE_LARGE : ulBlkSize;
                iRealRead = Fread(buf, sizeof(uchar), iExptRead, pPEInfo->fpFile);
                
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
        }
        */
    } catch(EXCEPT_MEM_ALLOC) {
        iRet = -1;        
    } catch(EXCEPT_IO_FILE_READ) {
        iRet = -1;
    } catch(EXCEPT_IO_FILE_SEEK) {
        iRet = -1;
    } end_try;

EXIT:    
    return rc;        
}
