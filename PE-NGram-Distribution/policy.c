#include "pe_info.h"
#include "policy.h"
#include "except.h"

int PolicySelectBlockRegions(PEInfo *pPEInfo, const char *cszPolicy, PolicyRegion **ppPolicyRegion) {
    int iRet;

    if (cszPolicy == NULL)
        iRet = GetRegsFromMaxEntropySec(pPEInfo, ppPolicyRegion);
    else {
        if (strcmp(cszPolicy, SELECT_PLATEAUS_WITHIN_MAX_ENTROPY_SEC) == 0)
            iRet = GetRegsFromMaxEntropySecWithinPlateaus(pPEInfo, ppPolicyRegion);
    }
    
    return iRet;
}

int GetRegsFromMaxEntropySec(PEInfo *pPEInfo, PolicyRegion **ppPolicyRegion) {
    int             i, iRet, idxSection;
    double          dAvg, dMax;
    SectionInfo     *pSection;
    PolicyRegion    *pPolicyRegion;
    Region          *arrRegion;
    
    iRet = 0;
    
    try {
        // Allocate the memory for PolicyRegion structure.
        *ppPolicyRegion = NULL;
        *ppPolicyRegion = (PolicyRegion*) Malloc(sizeof(PolicyRegion));
        pPolicyRegion = *ppPolicyRegion;
        
        pPolicyRegion->iCountRegions = 1;
        pPolicyRegion->arrRegion = NULL;
        pPolicyRegion->arrRegion = (Region*) Malloc(sizeof(Region));
        
        arrRegion = pPolicyRegion->arrRegion;
        arrRegion->listRangePair = NULL;
        arrRegion->listRangePair = (RangePair*) Malloc(sizeof(RangePair));
    
        // Select the block regions from the entire section with maximum average entropy.
        dMax = 0;
        idxSection = 0;
        for (i = 0 ; i < pPEInfo->pPEHeader->iCountSections ; i++) {
            pSection = pPEInfo->arrSectionInfo[i];
            
            // Skip the empty section.
            if (pSection->ulRawSize == 0)
                continue;
            
            dAvg = pSection->pEntropyInfo->dAvgEntropy;
            if (dAvg > dMax) {
                dMax = dAvg;
                idxSection = i;
            }            
        }
        arrRegion->idxSection = idxSection;
        arrRegion->listRangePair->idxBgn = 0;
        arrRegion->listRangePair->idxEnd = pPEInfo->arrSectionInfo[idxSection]->pEntropyInfo->ulSizeArray;
        arrRegion->listRangePair->next = NULL;
    } catch(EXCEPT_MEM_ALLOC) {
        iRet = -1;
    } end_try;
    
    return iRet;
}

int GetRegsFromMaxEntropySecWithinPlateaus(PEInfo *pPEInfo, PolicyRegion **ppPolicyRegion) {
    int             i, iRet, idxSection;
    ulong           ulSizeArray, ulIdx, ulPrevIdx, ulCount, ulIdxFront, ulIdxTail;
    double          dAvg, dMax, dDeviation, dPrevEntropy, dCurrEntropy, dAbs;
    double          *arrEntropy;
    PolicyRegion    *pPolicyRegion;
    SectionInfo     *pSection;
    EntropyInfo     *pEntropyInfo;
    Region          *arrRegion;
    RangePair       *currNode, *prevNode;
    
    iRet = 0;

    try {
        // Allocate the memory for PolicyRegion structure.
        *ppPolicyRegion = NULL;
        *ppPolicyRegion = (PolicyRegion*) Malloc(sizeof(PolicyRegion));
        pPolicyRegion = *ppPolicyRegion;
        
        pPolicyRegion->arrRegion = NULL;
        pPolicyRegion->arrRegion = (Region*) Malloc(sizeof(Region));        
        arrRegion = pPolicyRegion->arrRegion;
        arrRegion->listRangePair = NULL;    
            
        // Select the section with maximum average entropy.
        dMax = 0;
        idxSection = 0;
        for (i = 0 ; i < pPEInfo->pPEHeader->iCountSections ; i++) {
            pSection = pPEInfo->arrSectionInfo[i];
            
            // Skip the empty section.
            if (pSection->ulRawSize == 0)
                continue;
            
            dAvg = pSection->pEntropyInfo->dAvgEntropy;
            if (dAvg > dMax) {
                dMax = dAvg;
                idxSection = i;
            }
        }
        arrRegion->idxSection = idxSection;
        
        // Calculate the entropy deviation for the selected section.
        pEntropyInfo = pPEInfo->arrSectionInfo[idxSection]->pEntropyInfo;
        arrEntropy = pEntropyInfo->arrEntropy;
        dAvg = pEntropyInfo->dAvgEntropy;
        ulSizeArray = pEntropyInfo->ulSizeArray;
        ulIdx = 0;
        ulCount = 0;
        dDeviation = 0;
     
        while ((ulIdx < ulSizeArray) && (arrEntropy[ulIdx] < dAvg))
            ulIdx++;
        if (ulIdx == ulSizeArray)
            goto EXIT;
        dPrevEntropy = arrEntropy[ulIdx];
        ulPrevIdx = ulIdx;
        ulIdx++;
        
        while (ulIdx < ulSizeArray) {
            if (arrEntropy[ulIdx] >= dAvg) {
                if ((ulIdx - ulPrevIdx) == 1) {
                    dCurrEntropy = arrEntropy[ulIdx];
                    dDeviation += abs(dCurrEntropy - dPrevEntropy);
                    ulCount++;
                    dPrevEntropy = dCurrEntropy;
                }
                ulPrevIdx = ulIdx;
            }
            ulIdx++;
        }
        dDeviation /= ulCount;

        // Find the plateaus.
        ulIdxFront = ulIdxTail = 0;
        while (ulIdxTail < ulSizeArray) {
            if (arrEntropy[ulIdxTail] >= dAvg) {
                ulIdxFront = ulIdxTail;
                dPrevEntropy = arrEntropy[ulIdxFront];
                ulIdxTail++;

                while (ulIdxTail < ulSizeArray) {
                    dCurrEntropy = arrEntropy[ulIdxTail];
                    dAbs = abs(dCurrEntropy - dPrevEntropy);
                    if (dAbs > dDeviation) {
                        if (arrRegion->listRangePair == NULL) {
                            arrRegion->listRangePair = (RangePair*) Malloc(sizeof(RangePair));
                            currNode = arrRegion->listRangePair;
                            currNode->idxBgn = ulIdxFront;
                            currNode->idxEnd = ulIdxTail;
                            currNode->next = NULL;
                            prevNode = currNode;
                            currNode = currNode->next;
                        } else {
                            currNode = (RangePair*) Malloc(sizeof(RangePair));
                            currNode->idxBgn = ulIdxFront;
                            currNode->idxEnd = ulIdxTail;
                            currNode->next = NULL;
                            prevNode->next = currNode;
                            prevNode = currNode;
                            currNode = currNode->next;
                        }
                        break;
                    }
                    dPrevEntropy = dCurrEntropy;
                    ulIdxTail++;
                }
            } else 
                ulIdxTail++;
        }
        
        if (arrRegion->listRangePair != NULL)
            pPolicyRegion->iCountRegions = 1;
        else
            pPolicyRegion->iCountRegions = 0;
        
        printf("debug\n");
        currNode = arrRegion->listRangePair;
        while(currNode != NULL) {
            printf("0x%08x 0x%08x\n", currNode->idxBgn, currNode->idxEnd);
            currNode = currNode->next;
        }
        printf("end\n");
        
    } catch(EXCEPT_MEM_ALLOC) {
        iRet = -1;
    } end_try;
    
EXIT:
    return iRet;
}

int UninitPolicyRegion(PolicyRegion *pPolicyRegion) {
    int     i;
    Region  *arrRegion;
    
    if (pPolicyRegion != NULL) {
        arrRegion = pPolicyRegion->arrRegion;
        if (arrRegion != NULL) {
            for (i = 0 ; i < pPolicyRegion->iCountRegions ; i++) {
                if (arrRegion[i].listRangePair != NULL)
                    UninitListRangepair(arrRegion[i].listRangePair);
            }
            Free(arrRegion);
        }
        Free(pPolicyRegion);
    }
    
    return 0;
}

int UninitListRangepair(RangePair *curr) {
    if (curr != NULL) {
        UninitListRangepair(curr->next);
        Free(curr);
    }
}

int PolicyGenerateModel(PEInfo *pPEInfo, const char* cszPolicy, PolicyRegion *pPolicyRegion) {
    int iRet;
    
    if (cszPolicy == NULL)
        iRet = PutModelByDescFreqOrder(pPEInfo, pPolicyRegion);
    else {
    
    }
    
    return iRet;
}

int PutModelByDescFreqOrder(PEInfo *pPEInfo, PolicyRegion *pPolicyRegion) {
    int                 i, j, iRet, idxDenominator;
    ulong               ulNGramSpace, ulDomValue, ulDomFrequency, ulNumValue, ulNumFrequency;
    double              dFactor;
    NGramToken          *arrMergedToken;
    NGramModel          *pModel;
    NGramSingleSlice    *pSlices;
    
    iRet = 0;

    iRet = CollectTokensByPolicyRegions(pPEInfo, pPolicyRegion);
    if (iRet != 0)
        goto EXIT;
    
    iRet = MergeTokens(pPEInfo, &arrMergedToken);
    if (iRet != 0)
        goto EXIT;
        
    try {
        if (pPEInfo->pNGramModel == NULL)
            goto EXIT;
    
        // Generate model by descending order of the appearing frequencies of tokens.
        pModel = pPEInfo->pNGramModel;
        ulNGramSpace = pModel->ulNGramSpace;
        pModel->arrSlice = NULL;
        pModel->arrSlice = (NGramSingleSlice*) Calloc(ulNGramSpace, sizeof(NGramSingleSlice));
        pSlices = pModel->arrSlice;
        memset(pSlices, 0, sizeof(NGramSingleSlice) * ulNGramSpace);
        
        // Select the denominator.
        idxDenominator = 0;
        while ((arrMergedToken[idxDenominator].ulValue == 0) ||
                arrMergedToken[idxDenominator].ulValue == (ulNGramSpace - 1))
            idxDenominator++;
        
        // Collect the slices using the selected denominator.
        ulDomValue = arrMergedToken[idxDenominator].ulValue;
        ulDomFrequency = arrMergedToken[idxDenominator].ulFrequency;
        pModel->ulModelRealSize = 0;
        for (i = idxDenominator, j = 0 ; i < ulNGramSpace ; i++, j++) {
            ulNumValue = arrMergedToken[i].ulValue;
            ulNumFrequency = arrMergedToken[i].ulFrequency;

            if ((ulNumValue == 0) && (ulNumFrequency == 0))
                break;
            
            // Policy to truncate trailing noisy n-gram tokens.
            dFactor = (double)ulNumFrequency / (double)ulDomFrequency;
            if (dFactor < 0.05)
                break;
                
            pSlices[j].tokenDenominator.ulValue = ulDomValue;
            pSlices[j].tokenDenominator.ulFrequency = ulDomFrequency;
            pSlices[j].tokenNumerator.ulValue = ulNumValue;
            pSlices[j].tokenNumerator.ulFrequency = ulNumFrequency;
            pSlices[j].dFactor = dFactor;
            pModel->ulModelRealSize++;
        }
    } catch(EXCEPT_MEM_ALLOC) {
        iRet = -1;
    } end_try;
       
FREE_ARRAY:
    if (arrMergedToken != NULL)
        Free(arrMergedToken);
EXIT:
    return iRet;
}

int CollectTokensByPolicyRegions(PEInfo *pPEInfo, PolicyRegion *pPolicyRegion) {
    int             i, j, k, iRet, iCountSets, iNGramDimension, idxSection, idxBgn, idxEnd, iExptRead, iRealRead;
    int             idxByteVF, idxByteVT, idxBitVF, idxBitVT, idxByteCur, idxBitCur, iSftCount;
    uchar           ucMask, ucBit;
    ulong           ulSecRawSize, ulSecRawOffset, ulOffsetBgn, ulOffsetEnd, ulBlkSize;
    ulong           ulNGramSpace, ulIdxByteRF, ulNGramVal;
    Region          refRegion;
    NGramTokenSet   refSet;
    NGramModel      *pModel;
    RangePair       *listRangePair;
    uchar           buf[BUF_SIZE_LARGE];
    
    iRet = 0;
    
    try {
        // Allocate the memory for NGramModel structure.
        iCountSets = pPolicyRegion->iCountRegions;
        if (iCountSets == 0)
            goto EXIT;
        
        pPEInfo->pNGramModel = (NGramModel*) Malloc(sizeof(NGramModel));
        pModel = pPEInfo->pNGramModel;
        pModel->arrSlice = NULL;
        pModel->arrTokenSet = NULL;
        pModel->iCountSets = iCountSets;
        pModel->arrTokenSet = (NGramTokenSet*) Calloc(iCountSets, sizeof(NGramTokenSet));
        for (i = 0 ; i < iCountSets ; i++)
            pModel->arrTokenSet[i].arrToken = NULL;
        
        iNGramDimension = pPolicyRegion->iNGramDimension;
        ulNGramSpace = pow(UNI_GRAM_MAX_VALUE, iNGramDimension);
        pModel->ulNGramSpace = ulNGramSpace;
        iSftCount = iNGramDimension * SHIFT_RANGE_8BIT;

        // Inspect each selected section.
        for (i = 0 ; i < iCountSets ; i++) {
            refRegion = pPolicyRegion->arrRegion[i];
            refSet = pModel->arrTokenSet[i];
            
            // Allocate the memory to store n-gram tokens.
            refSet.arrToken = (NGramToken*) Calloc(ulNGramSpace, sizeof(NGramToken));
            memset(refSet.arrToken, 0, sizeof(NGramToken) * ulNGramSpace);

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
    } catch(EXCEPT_MEM_ALLOC) {
        iRet = -1;        
    } catch(EXCEPT_IO_FILE_READ) {
        iRet = -1;
    } catch(EXCEPT_IO_FILE_SEEK) {
        iRet = -1;
    } end_try;

EXIT:    
    return iRet;    
}

int MergeTokens(PEInfo *pPEInfo, NGramToken **pArrMergedTokens) {
    int             i, j, iRet;
    ulong           ulNGramSpace, ulValue, ulFrequency;
    NGramTokenSet   refSet;
    NGramToken      *arrMergedToken;
    
    iRet = 0;
    
    try {
        if (pPEInfo->pNGramModel == NULL)
            goto EXIT;
        
        // Allocate the memory to store merged tokens.
        ulNGramSpace = pPEInfo->pNGramModel->ulNGramSpace;
        *pArrMergedTokens = NULL;
        *pArrMergedTokens = (NGramToken*) Calloc(ulNGramSpace, sizeof(NGramToken));
        arrMergedToken = *pArrMergedTokens;
        memset(arrMergedToken, 0, sizeof(NGramToken) * ulNGramSpace);
        
        // Merge the tokens.
        for (i = 0 ; i < pPEInfo->pNGramModel->iCountSets ; i++) {
            refSet = pPEInfo->pNGramModel->arrTokenSet[i];
            
            for (j = 0 ; j < ulNGramSpace ; j++) {
                ulValue = refSet.arrToken[j].ulValue;
                ulFrequency = refSet.arrToken[j].ulFrequency;
                
                if((ulValue == 0) && (ulFrequency = 0))
                    break;
                
                arrMergedToken[ulValue].ulValue = ulValue;
                arrMergedToken[ulValue].ulFrequency += ulFrequency;
            }
        }

        // Sort the merged tokens.
        qsort(arrMergedToken, ulNGramSpace, sizeof(NGramToken), FuncCompareTokenDescOrder);
    } catch(EXCEPT_MEM_ALLOC) {
        iRet = -1;
    } end_try;

EXIT:
    return iRet;
}

// The comparison is for descending order sorting.
int FuncCompareTokenDescOrder(const void *pSrc, const void *pDst) {
    NGramToken *pSrcNGram, *pDstNGram;
    
    pSrcNGram = (NGramToken*)pSrc;
    pDstNGram = (NGramToken*)pDst;
    if(pSrcNGram->ulFrequency < pDstNGram->ulFrequency) {
        return 1;
    } else if(pSrcNGram->ulFrequency > pDstNGram->ulFrequency) {
        return -1;
    } else {
        return 0;
    }
}
