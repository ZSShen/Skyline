#include "util.h"
#include "except.h"
#include "pe_info.h"


void PEInfoInit(PEInfo *self) {
    
    self->szSampleName = NULL;
    self->fpSample = NULL;
    self->pPEHeader = NULL;
    self->arrSectionInfo = NULL;

    /* Let the function pointers point to the corresponding functions. */
    self->openSample = PEInfoOpenSample;
    self->parseHeaders = PEInfoParseHeaders;
    self->dump = PEInfoDump;

    return;
}


void PEInfoDeinit(PEInfo *self) {
    int i;

    if (self->szSampleName != NULL)
        Free(self->szSampleName);

    if (self->fpSample != NULL)
        Fclose(self->fpSample);
    
    /* Free all the SectionInfo structures. */
    if (self->arrSectionInfo != NULL) {
        for (i = 0 ; i < self->pPEHeader->ulNumSections ; i++) {
            if (self->arrSectionInfo[i] != NULL)        
                Free(self->arrSectionInfo[i]);
        }
        Free(self->arrSectionInfo);    
    }

    /* Free the PEHeader structure. */
    if (self->pPEHeader != NULL)
        Free(self->pPEHeader);

    return;
}

/**
 * PEInfoOpenSample(): Open the specified sample for analysis.
 */
int PEInfoOpenSample(PEInfo *self, const char *cszSamplePath) {
    int rc, idxFront, idxTail;

    try {
        rc = 0;
    
        /* Create the file pointer for the input sample. */        
        self->fpSample = Fopen(cszSamplePath, "rb");
        
        /* Extract the name of the input sample. */
        idxTail = strlen(cszSamplePath);
        idxFront = idxTail;
        while ((idxTail > 0) && (cszSamplePath[idxTail - 1] != '.'))
            idxTail--;
        if (idxTail == 0)
            idxTail = idxFront;
            
        idxFront = idxTail;
        idxTail--;    
        while ((idxFront > 0) && (cszSamplePath[idxFront - 1] != OS_PATH_SEPARATOR))
            idxFront--;

        self->szSampleName = (char*)Calloc((idxTail - idxFront + 1), sizeof(char));        
        MemCopy(self->szSampleName, cszSamplePath + idxFront, (idxTail - idxFront), sizeof(char));

    } catch(EXCEPT_MEM_ALLOC) {
        rc = -1;
    } catch(EXCEPT_IO_FILE_OPEN) {
        rc = -1;
    } end_try;

    return rc;
}


/**
 * PEInfoParseHeaders(): Collect the information from MZ header, PE header, PE option header,
 *                       and all the section headers.
 */
int PEInfoParseHeaders(PEInfo *self) {
    int     rc, i, j;
    ushort  ulWord;
    ulong   ulDword, ulOffset;
    size_t  nExptRead, nRealRead;
    uchar   *uszOriginalName;
    uchar   buf[BUF_SIZE_SMALL];
    
    rc = 0;
    try {
        /* Create the PEHeader structure. */
        self->pPEHeader = NULL;
        self->pPEHeader = (PEHeader*)Malloc(sizeof(PEHeader));
        
        //------------------------------------------------
        //  Examine DOS(MZ) header.
        //------------------------------------------------
        /* Check the MZ header. */
        nExptRead = DOS_HEADER_SIZE;
        nRealRead = Fread(buf, sizeof(uchar), nExptRead, self->fpSample);
        if ((nExptRead != nRealRead) || (buf[0] != 'M' || buf[1] != 'Z')) {
            Log0("Invalid PE file (Invalid MZ header).\n");
            rc = -1;
            goto EXIT;
        }
        
        /* Resolve the starting offset of PE header. */
        ulDword = 0;
        for (i = 1 ; i <= DATATYPE_SIZE_DWORD ; i++) {
            ulDword <<= SHIFT_RANGE_8BIT;
            ulDword += buf[DOS_HEADER_OFF_PE_HEADER_OFFSET + DATATYPE_SIZE_DWORD - i] & 0xff;
        }
        self->pPEHeader->ulHeaderOffset = ulDword;

        /* Move to the starting offset of PE header. */
        ulOffset = ulDword;
        Fseek(self->fpSample, ulOffset, SEEK_SET);
      
        //------------------------------------------------
        //  Examine PE header.
        //------------------------------------------------
        /* Check the PE header. */
        nExptRead = PE_HEADER_SIZE;
        nRealRead = Fread(buf, sizeof(uchar), nExptRead, self->fpSample);
        if ((nExptRead != nRealRead) || (buf[0] != 'P' || buf[1] != 'E')) {
            Log0("Invalid PE file (Invalid PE header).\n");
            rc = -1;
            goto EXIT;
        }
    
        /* Resolve the amount of sections. */
        ulWord = 0;
        for (i = 1 ; i <= DATATYPE_SIZE_WORD ; i++) {
            ulWord <<= SHIFT_RANGE_8BIT;
            ulWord += buf[PE_HEADER_OFF_NUMBER_OF_SECTIONS + DATATYPE_SIZE_WORD - i] & 0xff;
        }
        self->pPEHeader->ulNumSections = ulWord;

        /* Resolve the size of optional header. */
        ulWord = 0;
        for (i = 1 ; i <= DATATYPE_SIZE_WORD ; i++) {
            ulWord <<= SHIFT_RANGE_8BIT;
            ulWord += buf[PE_HEADER_OFF_SIZE_OF_OPT_HEADER + DATATYPE_SIZE_WORD - i] & 0xff;
        }
        
        /* Move to the starting offset of section headers. */
        ulOffset = self->pPEHeader->ulHeaderOffset + PE_HEADER_SIZE + ulWord;
        Fseek(self->fpSample, ulOffset, SEEK_SET);
        
        //------------------------------------------------
        //  Examine all the section headers.
        //------------------------------------------------
        /* Create the array to store the SectionInfo structure for each section. */
        ulWord = self->pPEHeader->ulNumSections;
        self->arrSectionInfo = (SectionInfo**)Calloc(ulWord, sizeof(SectionInfo*));
        memset(self->arrSectionInfo, (uint)NULL, sizeof(SectionInfo*) * ulWord);
        
        /* Traverse the section headers to collect the information from each section. */
        for (i = 0 ; i < ulWord ; i++) {
            nExptRead = SECTION_HEADER_PER_ENTRY_SIZE;
            nRealRead = Fread(buf, sizeof(uchar), nExptRead, self->fpSample);
            if (nExptRead != nRealRead) {
                Log0("Invalid PE file (Invalid section header).\n");
                rc = -1;
                goto EXIT;
            }
            
            /* Create the SectionInfo structure. */
            self->arrSectionInfo[i] = NULL;
            self->arrSectionInfo[i] = (SectionInfo*)Malloc(sizeof(SectionInfo));
            self->arrSectionInfo[i]->pEntropyInfo = NULL;
        
            /* Record the section name. */
            memset(self->arrSectionInfo[i]->uszOriginalName, 0, SECTION_HEADER_SECTION_NAME_SIZE + 1);
            memset(self->arrSectionInfo[i]->uszNormalizedName, 0, SECTION_HEADER_SECTION_NAME_SIZE + 1);            
        
            MemCopy(self->arrSectionInfo[i]->uszOriginalName, buf, SECTION_HEADER_SECTION_NAME_SIZE, sizeof(uchar));            
            uszOriginalName = self->arrSectionInfo[i]->uszOriginalName;
            for (j = 0 ; j < SECTION_HEADER_SECTION_NAME_SIZE ; j++) {
                if((uszOriginalName[j] >= 32) && (uszOriginalName[j] <= 126))
                    self->arrSectionInfo[i]->uszNormalizedName[j] = uszOriginalName[j];
                else
                    self->arrSectionInfo[i]->uszNormalizedName[j] = '_';
            }
            
            /* Record the raw section size. */
            ulDword = 0;
            for (j = 1 ; j <= DATATYPE_SIZE_DWORD ; j++) {
                ulDword <<= SHIFT_RANGE_8BIT;
                ulDword += buf[SECTION_HEADER_OFF_RAW_SIZE + DATATYPE_SIZE_DWORD - j] & 0xff;
            }
            self->arrSectionInfo[i]->ulRawSize = ulDword;
        
            /* Record the raw section offset. */
            ulDword = 0;
            for (j = 1 ; j <= DATATYPE_SIZE_DWORD ; j++) {
                ulDword <<= SHIFT_RANGE_8BIT;
                ulDword += buf[SECTION_HEADER_OFF_RAW_OFFSET + DATATYPE_SIZE_DWORD - j] & 0xff;
            }
            self->arrSectionInfo[i]->ulRawOffset = ulDword;

            /* Record the section characteristics. */
            ulDword = 0;
            for (j = 1 ; j <= DATATYPE_SIZE_DWORD ; j++) {
                ulDword <<= SHIFT_RANGE_8BIT;
                ulDword += buf[SECTION_HEADER_OFF_CHARS + DATATYPE_SIZE_DWORD - j] & 0xff;
            }
            self->arrSectionInfo[i]->ulCharacteristics = ulDword;
        }
    } catch(EXCEPT_MEM_ALLOC) {
        rc = -1;
    } catch(EXCEPT_IO_FILE_READ) {
        rc = -1;
    } catch(EXCEPT_IO_FILE_SEEK) {
        Log0("Invalid PE file (The target header can not be reached).\n");
        rc = -1;
    } end_try;
    
EXIT:
    return rc;
}


/**
 * PEInfoDump(): Dump the information recorded from the input sample for debug.
 */
void PEInfoDump(PEInfo *self) {
    int         i, j;    
    ushort      ulNumSections;
    SectionInfo *pSection;
    
    printf("Sample Name: %s\n", self->szSampleName);

    ulNumSections = self->pPEHeader->ulNumSections;
    printf("Total: %d sections\n\n", ulNumSections);
    for (i = 0 ; i < ulNumSections ; i++) {
        pSection = self->arrSectionInfo[i];
        
        printf("Section    Name: %s\n", pSection->uszNormalizedName);
        printf("Characteristics: 0x%08lx\n", pSection->ulCharacteristics);
        printf("Raw      Offset: 0x%08lx\n", pSection->ulRawOffset);
        printf("Raw        Size: 0x%08lx\n", pSection->ulRawSize);

        printf("\n");
    }
    
    return;
}

/*
int PEInfoInit(PEInfo **pSelf, const char *szInput) {
    int iRet, idxTail, idxFront;

    iRet = 0;
    
    try {
        // Allocate the memory for PEInfo structure.
        *pSelf = NULL;
        *pSelf = (PEInfo*) Malloc(sizeof(PEInfo));
        (*pSelf)->pPEHeader = NULL;
        (*pSelf)->arrSectionInfo = NULL;
        (*pSelf)->pNGramModel = NULL;

        // Extract the normalized sample name.
        idxTail = strlen(szInput);
        idxFront = idxTail;
        while ((idxTail > 0) && (szInput[idxTail - 1] != '.'))
            idxTail--;
        if (idxTail == 0)
            idxTail = idxFront;
            
        idxFront = idxTail;    
        while ((idxFront > 0) && (szInput[idxFront - 1] != OS_PATH_SEPARATOR))
            idxFront--;
        
        (*pSelf)->szSample = NULL;
        (*pSelf)->szSample = (char*) Calloc((idxTail - idxFront), sizeof(char));
        memset((*pSelf)->szSample, 0, (idxTail - idxFront) * sizeof(char));
        memcpy((*pSelf)->szSample, szInput + idxFront, (idxTail - idxFront - 1));
        
        // Open the file pointer for the input PE.
        (*pSelf)->fpFile = NULL;
        (*pSelf)->fpFile = Fopen(szInput, "rb");
    } catch(EXCEPT_MEM_ALLOC) {
        iRet = -1;
    } catch(EXCEPT_IO_FILE_OPEN) {
        Free(*pSelf);
        *pSelf = NULL;
        iRet = -1;
    } end_try;
    
    return iRet;
}


int PEInfoUninit(PEInfo *self) {
    int         i, iSize;
    SectionInfo *pSection;
    NGramModel  *pModel;
    
    if (self != NULL) {
        // Free the name buffer.
        if (self->szSample != NULL)
            Free(self->szSample);
    
        // Close the file pointer.
        if (self->fpFile != NULL)
            Fclose(self->fpFile);
      
        // Free the memory allocated by PEHeader structure.
        if (self->pPEHeader != NULL)
            Free(self->pPEHeader);
        
        // Free the memory allocated by SectionInfo related structures.
        if (self->arrSectionInfo != NULL) {
            for (i = 0 ; i < self->pPEHeader->iCountSections ; i++) {
                if (self->arrSectionInfo[i] != NULL) {
                    pSection = self->arrSectionInfo[i];
                    
                    if (pSection->pEntropyInfo != NULL) {
                        if (pSection->pEntropyInfo->arrEntropy != NULL)
                            Free(pSection->pEntropyInfo->arrEntropy);
                        Free(pSection->pEntropyInfo);
                    }        
                    
                    Free(self->arrSectionInfo[i]);
                }
            }
            Free(self->arrSectionInfo);
        }

        // Free the memory allocated by NGramModel related structures.
        if (self->pNGramModel != NULL) {
            pModel = self->pNGramModel;
            
            if (pModel->arrTokenSet != NULL) {
                for (i = 0 ; i < pModel->iCountSets ; i++) {
                    if (pModel->arrTokenSet[i].arrToken != NULL)
                        Free(pModel->arrTokenSet[i].arrToken);
                }
            }
            
            if (pModel->arrSlice != NULL)
                Free(pModel->arrSlice);

            Free(self->pNGramModel);
        }
        Free(self);
    }
    
    return 0;
}



int PEInfoCalculateSectionEntropy(PEInfo *self) {
    int         i, j, iRet, iRealRead, iExptRead, idxBlk;
    ulong       ulRawSize, ulRawOffset, ulCurrentRead;
    double      dEntropy, dMax, dAvg, dMin, dProb, dLogProb, dLogBase;
    SectionInfo *pSection;
    uchar       buf[ENTROPY_BLK_SIZE], refEntropy[ENTROPY_BLK_SIZE];
    
    iRet = 0;
    try {
        for (i = 0 ; i < self->pPEHeader->iCountSections ; i++) {
            pSection = self->arrSectionInfo[i];
            
            ulRawSize = pSection->ulRawSize;
            ulRawOffset = pSection->ulRawOffset;
            
            // Skip the empty section.
            if (ulRawSize == 0)
                continue;
                
            // Move to the starting offset of the designated section.
            //printf("0x%08x 0x%08x\n", ulRawSize, ulRawOffset);
            Fseek(self->fpFile, ulRawOffset, SEEK_SET);
            
            // Allocate the memory for EntropyInfo structure.
            pSection->pEntropyInfo = NULL;
            pSection->pEntropyInfo = (EntropyInfo*) Malloc(sizeof(EntropyInfo));
            pSection->pEntropyInfo->arrEntropy = NULL;
            if ((ulRawSize % ENTROPY_BLK_SIZE) == 0)
                pSection->pEntropyInfo->ulSizeArray = ulRawSize / ENTROPY_BLK_SIZE;
            else
                pSection->pEntropyInfo->ulSizeArray = ulRawSize / ENTROPY_BLK_SIZE + 1;
            pSection->pEntropyInfo->arrEntropy = NULL;
            pSection->pEntropyInfo->arrEntropy = (double*) Calloc(sizeof(double), pSection->pEntropyInfo->ulSizeArray);
            
            // Start the entropy calculation.
            ulCurrentRead = 0;
            idxBlk = 0;
            dMax = -1;
            dMin = 10;
            dAvg = 0;
            while (ulCurrentRead < ulRawSize) {
                iExptRead = ENTROPY_BLK_SIZE;
                if ((ulRawSize - ulCurrentRead) < iExptRead) 
                    iExptRead = ulRawSize - ulCurrentRead;
                
                // Read a chunk of binary.
                memset(buf, 0, sizeof(uchar) * ENTROPY_BLK_SIZE);
                iRealRead = Fread(buf, sizeof(uchar), iExptRead, self->fpFile);
                if (iExptRead != iRealRead) {
                    Log1("Invalid PE file (Invalid section \"%s\").\n", pSection->szNormalizedName);
                    iRet = -1;
                    goto EXIT;
                }
  
                // Calculate the entropy for this data chunk.
                memset(refEntropy, 0, sizeof(uchar) * ENTROPY_BLK_SIZE);
                for (j = 0 ; j < ENTROPY_BLK_SIZE ; j++) 
                    refEntropy[buf[j]]++;
                    
                dEntropy = 0;
                dLogBase = log(ENTROPY_LOG_BASE);
                for (j = 0 ; j < ENTROPY_BLK_SIZE ; j++) {
                    dProb = (double)refEntropy[j] / (double)ENTROPY_BLK_SIZE;
                    if (dProb > 0)
                        dLogProb = (double)log(dProb) / (double)dLogBase;
                    else
                        dLogProb = 0;
                    dEntropy += dProb * dLogProb;
                }
                dEntropy = -dEntropy;
                
                dAvg += dEntropy;
                if (dEntropy > dMax)
                    dMax = dEntropy;
                if (dEntropy < dMin)
                    dMin = dEntropy;
                
                pSection->pEntropyInfo->arrEntropy[idxBlk++] = dEntropy;
                ulCurrentRead += iRealRead;
            }
            
            pSection->pEntropyInfo->dMaxEntropy = dMax;
            pSection->pEntropyInfo->dMinEntropy = dMin;
            pSection->pEntropyInfo->dAvgEntropy = dAvg / idxBlk;
        }
    } catch(EXCEPT_IO_FILE_READ) {
        iRet = -1;
    } catch(EXCEPT_IO_FILE_SEEK) {
        Log0("Invalid PE file (The target section can not be reached).\n");
        iRet = -1;
    } end_try;
    
EXIT:
    return iRet;
}
*/
