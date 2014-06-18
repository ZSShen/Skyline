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
            Log1("The output folder path is too long (Longer than %d bytes).\n", BUF_SIZE_LARGE);
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
    int     rc, iLenPath;
    FILE    *fpReport;
    char    buf[BUF_SIZE_LARGE + 1];

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
            Log1("The output folder path is too long (Longer than %d bytes).\n", BUF_SIZE_LARGE);
            rc = -1;
            goto EXIT;
        }
        
        if (bHasSep == false)
            sprintf(buf, "%s%s%s", cszDirPath, cszSampleName, REPORT_PREFIX_TXT_SECTION_ENTROPY);
        else
            sprintf(buf, "%s%c%s%s", cszDirPath, OS_PATH_SEPARATOR, cszSampleName,
                    REPORT_PREFIX_TXT_NGRAM_MODEL);

        /* Prepare the file pointer for the report. */
        fpReport = Fopen(buf, "w");

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
int ReportPlotNGramModel(Report *self, const char *cszDirPath, const char *cszSampleName) {
    int rc;

    rc = 0;

    return rc;
}


