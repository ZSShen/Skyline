#include "debug.h"

int DumpPEInfo(PEInfo* pPEInfo) {
    int                 i, j ,iCountSections;
    ulong               ulModelRealSize;
    SectionInfo         *pSection;
    EntropyInfo         *pEntropyInfo;
    NGramSingleSlice    *pSlices;
    
    iCountSections = pPEInfo->pPEHeader->iCountSections;
    printf("Total: %d sections\n\n", iCountSections);
    
    for (i = 0 ; i < iCountSections ; i++) {
        pSection = pPEInfo->arrSectionInfo[i];
        
        printf("Section    Name: %s\n", pSection->szNormalizedName);
        printf("Characteristics: 0x%08x\n", pSection->ulCharacteristics);
        printf("Raw      Offset: 0x%08x\n", pSection->ulRawOffset);
        printf("Raw        Size: 0x%08x\n", pSection->ulRawSize);
        
        pEntropyInfo = pSection->pEntropyInfo;
        printf("Max Entropy: %.3lf\n", pEntropyInfo->dMaxEntropy);
        printf("Avg Entropy: %.3lf\n", pEntropyInfo->dAvgEntropy);
        printf("Min Entropy: %.3lf\n", pEntropyInfo->dMinEntropy);
        
        for (j = 0 ; j < pEntropyInfo->ulSizeArray ; j++)
            printf("\t%d\t%.3lf\n", j, pEntropyInfo->arrEntropy[j]);
        
        printf("\n");
    }
    
    ulModelRealSize = pPEInfo->pNGramModel->ulModelRealSize;
    pSlices = pPEInfo->pNGramModel->arrSlice;
    printf("\n----------------------------------------------\n");
    for (i = 0 ; i < ulModelRealSize ; i++) {
        printf("%.5lf  [0x%08x:%d]  [0x%08x:%d]\n", pSlices[i].dFactor, 
                                                    pSlices[i].tokenNumerator.ulValue,
                                                    pSlices[i].tokenNumerator.ulFrequency,
                                                    pSlices[i].tokenDenominator.ulValue,
                                                    pSlices[i].tokenDenominator.ulFrequency);
    }
    return 0;
}