#include "report.h"


/* Constructor for Report structure. */
void ReportInit(Report *self) {

    self->generateFolder = ReportGenerateFolder;
    self->logEntropyDistribution = ReportLogEntropyDistribution;
    self->logNGramModel = ReportLogNGramModel;
    self->plotNGramModel = ReportPlotNGramModel;

    return;
}

/* Destructor for Report structure. */
void ReportDeinit(Report *self) {

    return;
}

int ReportGenerateFolder(Report *self, const char *cszDirPath) {
    int     rc, state, iLenPath, iLenRest;
    DIR     *dir;
    char    szPathReport[BUF_SIZE_MID + 1];
    struct dirent *entry;

    rc = 0;
    try {
        #if defined(_WIN32)

        #elif defined(__linux__)
            dir = NULL;
            state = Mkdir(cszDirPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

            /* The folder already exists and we should remove all the files residing in it. */
            if (state != 0) {
                dir = Opendir(cszDirPath);
                if (dir != NULL) {
                    iLenPath = strlen(cszDirPath);
                    memset(szPathReport, 0, sizeof(char) * (BUF_SIZE_MID + 1));
                    strcpy(szPathReport, cszDirPath);

                    if (cszDirPath[iLenPath - 1] != OS_PATH_SEPARATOR) {
                        szPathReport[iLenPath] = OS_PATH_SEPARATOR;
                        iLenPath++;
                    }

                    while ((entry = Readdir(dir)) != NULL) {
                        if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0))
                            continue;
                        strcpy(szPathReport + iLenPath, entry->d_name);
                        Unlink(szPathReport);
                        iLenRest = strlen(entry->d_name);
                        memset(szPathReport + iLenPath, 0, sizeof(char) * iLenRest);
                    }
                }
            }
        #endif
    } catch(EXCEPT_IO_DIR_MAKE) {
        rc = -1;
    } catch(EXCEPT_IO_DIR_OPEN) {
        rc = -1;
    } catch(EXCEPT_IO_DIR_READ) {
        rc = -1;
    } catch(EXCEPT_IO_FILE_UNLINK) {
        rc = -1;
    } end_try;

    if (dir != NULL)
        Closedir(dir);
EXIT:
    return rc;
}

int ReportLogEntropyDistribution(Report *self, PEInfo *pPEInfo, const char *cszDirPath, const char *cszSampleName) {
    bool        bHasSep;
    int         rc, i, j, iLenPath, iLenBuf, iCountBatch;
    ushort      usNumSections;
    ulong       ulSizeSection;
    FILE        *fpReport;
    SectionInfo *pSection;
    EntropyInfo *pEntropy;
    char        buf[BUF_SIZE_LARGE + 1], szPathReport[BUF_SIZE_MID + 1];

    rc = 0;
    try {
        /* Generate the report path string. */
        bHasSep = false;
        iLenPath = strlen(cszDirPath);
        if (cszDirPath[iLenPath - 1] == OS_PATH_SEPARATOR) {
            iLenPath++;
            bHasSep = true;
        }
        iLenPath += strlen(cszSampleName);
        iLenPath += strlen(REPORT_POSTFIX_TXT_SECTION_ENTROPY);

        if (iLenPath > BUF_SIZE_MID) {
            Log1("The file path is too long (Maximum allowed length is %d bytes).\n", BUF_SIZE_MID);
            rc = -1;
            goto EXIT;
        }

        memset(szPathReport, 0, sizeof(char) * (BUF_SIZE_MID + 1));
        if (bHasSep == true)
            sprintf(szPathReport, "%s%s%s", cszDirPath, cszSampleName, REPORT_POSTFIX_TXT_SECTION_ENTROPY);
        else
            sprintf(szPathReport, "%s%c%s%s", cszDirPath, OS_PATH_SEPARATOR, cszSampleName,
                    REPORT_POSTFIX_TXT_SECTION_ENTROPY);

        /* Prepare the file pointer for the report. */
        fpReport = Fopen(szPathReport, "w");

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
        Fclose(fpReport);
        rc = -1;
    } end_try;

EXIT:
    return rc;
}

int ReportLogNGramModel(Report *self, NGram *pNGram, const char *cszDirPath, const char *cszSampleName) {
    bool    bHasSep;
    int     rc, i, iLenPath, iLenBuf, iCountBatch;
    FILE    *fpReport;
    Slice   **arrSlice;
    char    buf[BUF_SIZE_LARGE + 1], szPathReport[BUF_SIZE_MID + 1];

    rc = 0;
    try {
        /* Generate the report path string. */
        bHasSep = false;
        iLenPath = strlen(cszDirPath);
        if (cszDirPath[iLenPath - 1] == OS_PATH_SEPARATOR) {
            iLenPath++;
            bHasSep = true;
        }
        iLenPath += strlen(cszSampleName);
        iLenPath += strlen(REPORT_POSTFIX_TXT_NGRAM_MODEL);

        if (iLenPath > BUF_SIZE_MID) {
            Log1("The file path is too long (Maximum allowed length is %d bytes).\n", BUF_SIZE_MID);
            rc = -1;
            goto EXIT;
        }

        memset(szPathReport, 0, sizeof(char) * (BUF_SIZE_MID + 1));
        if (bHasSep == true)
            sprintf(szPathReport, "%s%s%s", cszDirPath, cszSampleName, REPORT_POSTFIX_TXT_NGRAM_MODEL);
        else
            sprintf(szPathReport, "%s%c%s%s", cszDirPath, OS_PATH_SEPARATOR, cszSampleName,
                    REPORT_POSTFIX_TXT_NGRAM_MODEL);

        /* Prepare the file pointer for the report. */
        fpReport = Fopen(szPathReport, "w");

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

            if (arrSlice[i]->dScore < TRUNCATE_THRESHOLD)
                break;

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
        Fclose(fpReport);
        rc = -1;
    } end_try;

EXIT:
    return rc;
}

int ReportPlotNGramModel(Report *self, NGram *pNGram, const char *cszDirPath, const char *cszSampleName) {
    bool    bHasSep;
    int     rc, state, iLenPath, iLenBuf;
    FILE    *fpScript, *fpProc;
    char    buf[BUF_SIZE_LARGE];
    char    szPathLog[BUF_SIZE_MID + 1], szPathScript[BUF_SIZE_MID + 1], szPathImage[BUF_SIZE_MID + 1];

    rc = 0;
    try {
        /* Check if the raw n-gram dump exists. */
        bHasSep = false;
        iLenPath = strlen(cszDirPath);
        if (cszDirPath[iLenPath - 1] == OS_PATH_SEPARATOR) {
            iLenPath++;
            bHasSep = true;
        }
        iLenPath += strlen(cszSampleName);
        iLenPath += strlen(REPORT_POSTFIX_TXT_NGRAM_MODEL);

        if (iLenPath > BUF_SIZE_MID) {
            Log1("The file path is too long (Maximum allowed length is %d bytes).\n", BUF_SIZE_MID);
            rc = -1;
            goto EXIT;
        }

        memset(szPathLog, 0, sizeof(char) * (BUF_SIZE_MID + 1));
        if (bHasSep == true)
            sprintf(szPathLog, "%s%s%s", cszDirPath, cszSampleName, REPORT_POSTFIX_TXT_NGRAM_MODEL);
        else
            sprintf(szPathLog, "%s%c%s%s", cszDirPath, OS_PATH_SEPARATOR, cszSampleName,
                    REPORT_POSTFIX_TXT_NGRAM_MODEL);

        #if defined(_WIN32)

        #elif defined(__linux__)
            state = access(szPathLog, F_OK);
        #endif

        if (state == -1) {
            Log1("The raw n-gram dump at %s does not exist.\n", szPathLog);
            goto EXIT;
        }

        /* Generate the path string for the outputted image. */
        bHasSep = false;
        iLenPath = strlen(cszDirPath);
        if (cszDirPath[iLenPath - 1] == OS_PATH_SEPARATOR) {
            iLenPath++;
            bHasSep = true;
        }
        iLenPath += strlen(cszSampleName);
        iLenPath += strlen(REPORT_POSTFIX_PNG_NGRAM_MODEL);

        if (iLenPath > BUF_SIZE_MID) {
            Log1("The file path is too long (Maximum allowed length is %d bytes).\n", BUF_SIZE_MID);
            rc = -1;
            goto EXIT;
        }

        memset(szPathImage, 0, sizeof(char) * (BUF_SIZE_MID + 1));
        if (bHasSep == true)
            sprintf(szPathImage, "%s%s%s", cszDirPath, cszSampleName, REPORT_POSTFIX_PNG_NGRAM_MODEL);
        else
            sprintf(szPathImage, "%s%c%s%s", cszDirPath, OS_PATH_SEPARATOR, cszSampleName,
                    REPORT_POSTFIX_PNG_NGRAM_MODEL);

        /* Generate the path string for the gnuplot script. */
        bHasSep = false;
        iLenPath = strlen(cszDirPath);
        if (cszDirPath[iLenPath - 1] == OS_PATH_SEPARATOR) {
            iLenPath++;
            bHasSep = true;
        }
        iLenPath += strlen(cszSampleName);
        iLenPath += strlen(REPORT_POSTFIX_GNU_PLOT_SCRIPT);

        if (iLenPath > BUF_SIZE_MID) {
            Log1("The file path is too long (Maximum allowed length is %d bytes).\n", BUF_SIZE_MID);
            rc = -1;
            goto EXIT;
        }

        memset(szPathScript, 0, sizeof(char) * (BUF_SIZE_MID + 1));
        if (bHasSep == true)
            sprintf(szPathScript, "%s%s%s", cszDirPath, cszSampleName, REPORT_POSTFIX_GNU_PLOT_SCRIPT);
        else
            sprintf(szPathScript, "%s%c%s%s", cszDirPath, OS_PATH_SEPARATOR, cszSampleName,
                    REPORT_POSTFIX_GNU_PLOT_SCRIPT);

        /* Prepare the file pointer for the script. */
        fpScript = Fopen(szPathScript, "w");

        /* Compile the drawing commands. */
        memset(buf, 0, sizeof(char) * BUF_SIZE_LARGE);
        iLenBuf = 0;

        sprintf(buf, "set terminal png size %d, %d\n", REPORT_IMAGE_SIZE_WIDTH, REPORT_IMAGE_SIZE_HEIGHT);
        iLenBuf = strlen(buf);

        #if defined(_WIN32)

        #elif defined(__linux__)
            sprintf(buf + iLenBuf, "set output \"%s\"\n", szPathImage);
        #endif
        iLenBuf = strlen(buf);

        sprintf(buf + iLenBuf, "set title \"%s\"\n", cszSampleName);
        iLenBuf = strlen(buf);

        sprintf(buf + iLenBuf, "set xlabel \"%s\"\nset ylabel \"%s\"\n", REPORT_IMAGE_X_AXIS, REPORT_IMAGE_Y_AXIS);
        iLenBuf = strlen(buf);

        #if defined(_WIN32)

        #elif defined(__linux__)
            sprintf(buf + iLenBuf, "plot \"%s\" title \"\" with lines\nexit\n", szPathLog);
        #endif
        iLenBuf = strlen(buf);
        Fwrite(buf, sizeof(char), iLenBuf, fpScript);
        Fclose(fpScript);

        /* Execute the gnuplot script. */
        #if defined(_WIN32)

        #elif defined(__linux__)
            memset(buf, 0, sizeof(char) * (BUF_SIZE_MID + 100));
            sprintf(buf, "%s %s", PATH_GNUPLOT_LINUX, szPathScript);
            fpProc = Popen(buf, "r");
            Pclose(fpProc);
        #endif
    } catch(EXCEPT_IO_DIR_MAKE) {
        rc = -1;
    } catch(EXCEPT_IO_FILE_WRITE) {
        rc = -1;
    } catch(EXCEPT_PROC_OPEN) {
        rc = -1;
    } catch(EXCEPT_PROC_CLOSE) {
        rc = -1;
    } end_try;

EXIT:
    return rc;
}