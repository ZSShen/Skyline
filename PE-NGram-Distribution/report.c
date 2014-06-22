#include "report.h"


/* Constructor for Report structure. */
void ReportInit(Report *self) {

    self->logEntropyDistribution = ReportLogEntropyDistribution;
    self->logNGramModel = ReportLogNGramModel;
    self->plotNGramModel = ReportPlotNGramModel;

    return;
}


/* Destructor for Report structure. */
void ReportDeinit(Report *self) {

    return;
}


/**
 * ReportLogEntropyDistribution(): Log the entropy distribution of all the sections.
 */
int ReportLogEntropyDistribution(Report *self, PEInfo *pPEInfo, const char *cszDirPath, const char *cszSampleName) {
    bool        bHasSep;    
    int         rc, i, j, iLenPath, iLenBuf, iCountBatch;
    ushort      usNumSections;
    ulong       ulSizeSection;
    FILE        *fpReport;
    SectionInfo *pSection;
    EntropyInfo *pEntropy;
    char        buf[BUF_SIZE_LARGE + 1];

    rc = 0;

    try {
        #if defined(_WIN32)

        #elif defined(__linux__)
            Mkdir(cszDirPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        #endif

        /* Generate the report path string. */
        memset(buf, 0, sizeof(char) * (BUF_SIZE_LARGE + 1));
    
        bHasSep = false;
        iLenPath = strlen(cszDirPath);
        if (cszDirPath[iLenPath - 1] != OS_PATH_SEPARATOR) {
            iLenPath++;
            bHasSep = true;        
        }
        iLenPath += strlen(cszSampleName);
        iLenPath += strlen(REPORT_PREFIX_TXT_SECTION_ENTROPY);

        if (iLenPath > BUF_SIZE_LARGE) {
            Log1("The file path is too long (Longer than %d bytes).\n", BUF_SIZE_LARGE);
            rc = -1;
            goto EXIT;
        }
        
        if (bHasSep == false)
            sprintf(buf, "%s%s%s", cszDirPath, cszSampleName, REPORT_PREFIX_TXT_SECTION_ENTROPY);
        else
            sprintf(buf, "%s%c%s%s", cszDirPath, OS_PATH_SEPARATOR, cszSampleName,
                    REPORT_PREFIX_TXT_SECTION_ENTROPY);
        
        /* Prepare the file pointer for the report. */
        fpReport = Fopen(buf, "w");

        /* Log the total number of sections. */
        usNumSections = pPEInfo->pPEHeader->usNumSections;
        memset(buf, 0, sizeof(char) * BUF_SIZE_LARGE);
        sprintf(buf, "Total: %d sections.\n\n", usNumSections);
        iLenBuf = strlen(buf);
        Fwrite(buf, sizeof(char), iLenBuf, fpReport);

        /* Walk through each section and log the relevant data. */
        for (i = 0 ; i < usNumSections ; i++) {
            pSection = pPEInfo->arrSectionInfo[i];
            ulSizeSection = pSection->ulRawSize;
            
            /* Log the section attributes. */
            memset(buf, 0, sizeof(char) * BUF_SIZE_LARGE);
            sprintf(buf, "[Section #%d]\n", i);
            iLenBuf = strlen(buf);            
            sprintf(buf + iLenBuf, "Section    Name: %s\n", pSection->uszNormalizedName);               
            iLenBuf = strlen(buf);
            sprintf(buf + iLenBuf, "Characteristics: 0x%08lx\n", pSection->ulCharacteristics);
            iLenBuf = strlen(buf);
            sprintf(buf + iLenBuf, "Raw      Offset: 0x%08lx\n", pSection->ulRawOffset);
            iLenBuf = strlen(buf);
            sprintf(buf + iLenBuf, "Raw        Size: 0x%08lx\n", pSection->ulRawSize);
            iLenBuf = strlen(buf);            

            pEntropy = pSection->pEntropyInfo;
            if (pSection->ulRawSize == 0) {
                sprintf(buf + iLenBuf, "\tThe empty section.\n\n");
                iLenBuf = strlen(buf);
            } else {
                sprintf(buf + iLenBuf, "\tMax Entropy: %.3lf\n", pEntropy->dMaxEntropy);
                iLenBuf = strlen(buf);            
                sprintf(buf + iLenBuf, "\tAvg Entropy: %.3lf\n", pEntropy->dAvgEntropy);
                iLenBuf = strlen(buf);            
                sprintf(buf + iLenBuf, "\tMin Entropy: %.3lf\n", pEntropy->dMinEntropy);
                iLenBuf = strlen(buf);            
            }
            Fwrite(buf, sizeof(char), iLenBuf, fpReport);
        
            /* Log the entropy distribution if the section is not empty. */
            if (pEntropy != NULL) {
                iLenBuf = iCountBatch = 0;                
                memset(buf, 0, sizeof(char) * BUF_SIZE_LARGE);
                for (j = 0 ; j < pEntropy->ulNumBlks ; j++) {
                    sprintf(buf + iLenBuf, "\t\tBlk #%d: %.3lf\n", j, pEntropy->arrEntropy[j]);
                    iLenBuf = strlen(buf);
                    iCountBatch++;
                    if (iCountBatch == BATCH_WRITE_LINE_COUNT) {
                        Fwrite(buf, sizeof(char), iLenBuf, fpReport);
                        iLenBuf = iCountBatch = 0;                    
                        memset(buf, 0, sizeof(char) * BUF_SIZE_LARGE);
                    }
                }
                buf[iLenBuf++] = '\n';
                Fwrite(buf, sizeof(char), iLenBuf, fpReport);
            }
        }

        /* Release the file pointer. */
        Fclose(fpReport);

    } catch(EXCEPT_IO_DIR_MAKE) {
        rc = -1;
    } catch(EXCEPT_IO_FILE_WRITE) {
        rc = -1;
    } end_try;

EXIT:
    return rc;
}


/**
 * ReportLogNGramModel(): Log the full n-gram model.
 */
int ReportLogNGramModel(Report *self, NGram *pNGram, const char *cszDirPath, const char *cszSampleName) {
    bool    bHasSep;
    int     rc, i, iLenPath, iLenBuf, iCountBatch;
    FILE    *fpReport;
    Slice   **arrSlice;
    char    buf[BUF_SIZE_LARGE + 1];

    rc = 0;

    try {
        #if defined(_WIN32)

        #elif defined(__linux__)
            Mkdir(cszDirPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        #endif
    
        //---------------------------------------------------------
        //  First phase: Prepare the drawing commands for gunplot.
        //---------------------------------------------------------
        /* Generate the report path string. */
        memset(buf, 0, sizeof(char) * (BUF_SIZE_LARGE + 1));
    
        bHasSep = false;
        iLenPath = strlen(cszDirPath);
        if (cszDirPath[iLenPath - 1] != OS_PATH_SEPARATOR) {
            iLenPath++;
            bHasSep = true;        
        }
        iLenPath += strlen(cszSampleName);
        iLenPath += strlen(REPORT_PREFIX_TXT_NGRAM_MODEL);

        if (iLenPath > BUF_SIZE_LARGE) {
            Log1("The file path is too long (Longer than %d bytes).\n", BUF_SIZE_LARGE);
            rc = -1;
            goto EXIT;
        }
        
        if (bHasSep == false)
            sprintf(buf, "%s%s%s", cszDirPath, cszSampleName, REPORT_PREFIX_TXT_NGRAM_MODEL);
        else
            sprintf(buf, "%s%c%s%s", cszDirPath, OS_PATH_SEPARATOR, cszSampleName,
                    REPORT_PREFIX_TXT_NGRAM_MODEL);

        /* Prepare the file pointer for the report. */
        fpReport = Fopen(buf, "w");

        /* Log each piece of n-gram model slice. */
        arrSlice = pNGram->arrSlice;
        
        iLenBuf = iCountBatch = 0;
        memset(buf, 0, sizeof(char) * BUF_SIZE_LARGE);    
        for (i = 0 ; i < pNGram->ulNumSlices ; i++) {
            sprintf(buf + iLenBuf, "%d\t%.3lf\t#(0x%08lx:%lu)\t(0x%08lx:%lu)\n", i, arrSlice[i]->dScore,
                    arrSlice[i]->pNumerator->ulValue,
                    arrSlice[i]->pNumerator->ulFrequency,
                    arrSlice[i]->pDenominator->ulValue,
                    arrSlice[i]->pDenominator->ulFrequency);            
            iLenBuf = strlen(buf);
            iCountBatch++;
            if (iCountBatch == BATCH_WRITE_LINE_COUNT) {
                Fwrite(buf, sizeof(char), iLenBuf, fpReport);
                iLenBuf = iCountBatch = 0;
                memset(buf, 0, sizeof(char) * BUF_SIZE_LARGE);                
            }
        }
        Fwrite(buf, sizeof(char), iLenBuf, fpReport);

        /* Release the file pointer. */
        Fclose(fpReport);
        
    } catch(EXCEPT_IO_DIR_MAKE) {
        rc = -1;
    } catch(EXCEPT_IO_FILE_WRITE) {
        rc = -1;
    } end_try;

EXIT:
    return rc;
}


/**
 * ReportPlotNGramModel(): Plot the visualized trend line of n-gram model with gnuplot utility.
 */
int ReportPlotNGramModel(Report *self, NGram *pNGram, const char *cszDirPath, const char *cszSampleName) {
    bool    bHasSep;
    int     rc, state, iLenPath, iLenBuf;
    FILE    *fpScript;
    char    buf[BUF_SIZE_LARGE], bufHelp[BUF_SIZE_LARGE];

    rc = 0;

    try {
        #if defined(_WIN32)

        #elif defined(__linux__)
            Mkdir(cszDirPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        #endif

        /* Check if the raw n-gram dump exists. */
        memset(bufHelp, 0, sizeof(char) * (BUF_SIZE_LARGE + 1));
        bHasSep = false;
        iLenPath = strlen(cszDirPath);
        if (cszDirPath[iLenPath - 1] != OS_PATH_SEPARATOR) {
            iLenPath++;
            bHasSep = true;        
        }
        iLenPath += strlen(cszSampleName);
        iLenPath += strlen(REPORT_PREFIX_TXT_NGRAM_MODEL);

        if (iLenPath > BUF_SIZE_LARGE) {
            Log1("The file path is too long (Longer than %d bytes).\n", BUF_SIZE_LARGE);
            rc = -1;
            goto EXIT;
        }
        
        if (bHasSep == false)
            sprintf(bufHelp, "%s%s%s", cszDirPath, cszSampleName, REPORT_PREFIX_TXT_NGRAM_MODEL);
        else
            sprintf(bufHelp, "%s%c%s%s", cszDirPath, OS_PATH_SEPARATOR, cszSampleName,
                    REPORT_PREFIX_TXT_NGRAM_MODEL);
        
        state = access(bufHelp, F_OK);        
        if (state == -1) {
            Log1("The raw n-gram dump at %s does not exist.\n", bufHelp);
            goto EXIT;
        }

        /* Generate the script path string. */
        memset(buf, 0, sizeof(char) * (BUF_SIZE_LARGE + 1));
    
        bHasSep = false;
        iLenPath = strlen(cszDirPath);
        if (cszDirPath[iLenPath - 1] != OS_PATH_SEPARATOR) {
            iLenPath++;
            bHasSep = true;        
        }
        iLenPath += strlen(cszSampleName);
        iLenPath += strlen(REPORT_PREFIX_GNU_PLOT_SCRIPT);

        if (iLenPath > BUF_SIZE_LARGE) {
            Log1("The file path is too long (Longer than %d bytes).\n", BUF_SIZE_LARGE);
            rc = -1;
            goto EXIT;
        }
        
        if (bHasSep == false)
            sprintf(buf, "%s%s%s", cszDirPath, cszSampleName, REPORT_PREFIX_GNU_PLOT_SCRIPT);
        else
            sprintf(buf, "%s%c%s%s", cszDirPath, OS_PATH_SEPARATOR, cszSampleName,
                    REPORT_PREFIX_GNU_PLOT_SCRIPT);

        /* Prepare the file pointer for the script. */
        fpScript = Fopen(buf, "w");
        memset(buf, 0, sizeof(char) * BUF_SIZE_LARGE);
        
        /* 1. Set the size of output image file. */
        sprintf(buf, "set terminal png size %d, %d\n", REPORT_IMAGE_SIZE_WIDTH, REPORT_IMAGE_SIZE_HEIGHT);
        iLenBuf = strlen(buf);

        

        /* Release the file pointer. */
        Fclose(fpScript);
        
    } catch(EXCEPT_IO_DIR_MAKE) {
        rc = -1;
    } catch(EXCEPT_IO_FILE_WRITE) {
        rc = -1;
    } end_try;

EXIT:
    return rc;
}


