#include "util.h"
#include "except.h"
#include "pe_info.h"
#include "report.h"

int ReportGenerate(PEInfo *pPEInfo, const char *cszOutputFolder) {
    int iRet, iStatus;
    
    iRet = 0;
    
    // Create the report folder if it does not exists.
    iStatus = GetFileAttributes(cszOutputFolder);
    if (iStatus != FILE_ATTRIBUTE_DIRECTORY) {
        iStatus = CreateDirectory(cszOutputFolder, NULL);
        if (iStatus == 0) {
            Log1("Can not create the output folder. (Error Code: %d)\n", GetLastError());
            iRet = -1;
            goto EXIT;
        }
    }
    // Generate the summary report for the entropy distribution of each section.
    iRet = LogSectionEntropy(pPEInfo, cszOutputFolder);
    if (iRet != 0)
        goto EXIT;

    // Generate the text dump for the n-gram model.
    iRet = LogNGramModel(pPEInfo, cszOutputFolder);
    if (iRet != 0)
        goto EXIT;
    
    // Plot the image for the n-gram model.
    iRet = PlotNGramModel(pPEInfo, cszOutputFolder);

EXIT:
    return iRet;
}

int LogSectionEntropy(PEInfo *pPEInfo, const char *cszOutputFolder) {
    int         i, j, iRet, iLenFolder, iLenFile, idxBuf, iCountSections, iCountBatch;
    char        bufMid[BUF_SIZE_MID], bufBig[BUF_SIZE_LARGE];
    FILE        *fpOutput;
    SectionInfo *pSection;
    EntropyInfo *pEntropyInfo;
    
    iRet = 0;
    
    // Generate the path string of the output text file.
    memset(bufMid, 0, BUF_SIZE_MID * sizeof(char));
    iLenFolder = strlen(cszOutputFolder);
    if (cszOutputFolder[iLenFolder - 1] == OS_PATH_SEPARATOR)
        snprintf(bufMid, BUF_SIZE_MID, "%s%s", cszOutputFolder, 
                                               REPORT_NAME_TXT_SECTION_ENTROPY);
    else
        snprintf(bufMid, BUF_SIZE_MID, "%s%c%s", cszOutputFolder,
                                                 OS_PATH_SEPARATOR,
                                                 REPORT_NAME_TXT_SECTION_ENTROPY);

    try {
        fpOutput = NULL;
        fpOutput = Fopen(bufMid, "w");

        // Print the total number of sections.
        iCountSections = pPEInfo->pPEHeader->iCountSections;
        memset(bufBig, 0, BUF_SIZE_LARGE * sizeof(char));
        snprintf(bufBig, BUF_SIZE_LARGE, "Total: %d sections. \n", iCountSections);
        idxBuf = strlen(bufBig);
        
        for (i = 0 ; i < iCountSections ; i++) {
            pSection = pPEInfo->arrSectionInfo[i];
        
            // Skip the empty section.
            if (pSection->ulRawSize == 0)
                continue;

            // Print the section attribute and summarized entropy information.
            snprintf(bufBig + idxBuf, BUF_SIZE_LARGE, "\nSection    Name: %s\n", pSection->szNormalizedName);
            idxBuf = strlen(bufBig);            
            snprintf(bufBig + idxBuf, BUF_SIZE_LARGE, "Characteristics: 0x%08x\n", pSection->ulCharacteristics);
            idxBuf = strlen(bufBig);            
            snprintf(bufBig + idxBuf, BUF_SIZE_LARGE, "Raw      Offset: 0x%08x\n", pSection->ulRawOffset);
            idxBuf = strlen(bufBig);            
            snprintf(bufBig + idxBuf, BUF_SIZE_LARGE, "Raw        Size: 0x%08x\n", pSection->ulRawSize);
            idxBuf = strlen(bufBig);            
            
            pEntropyInfo = pSection->pEntropyInfo;
            snprintf(bufBig + idxBuf, BUF_SIZE_LARGE, "Max Entropy: %.3lf\n", pEntropyInfo->dMaxEntropy);
            idxBuf = strlen(bufBig);            
            snprintf(bufBig + idxBuf, BUF_SIZE_LARGE, "Avg Entropy: %.3lf\n", pEntropyInfo->dAvgEntropy);
            idxBuf = strlen(bufBig);            
            snprintf(bufBig + idxBuf, BUF_SIZE_LARGE, "Min Entropy: %.3lf\n", pEntropyInfo->dMinEntropy);
            idxBuf = strlen(bufBig);            
            
            Fwrite(bufBig, sizeof(char), idxBuf, fpOutput);
            memset(bufBig, 0, BUF_SIZE_LARGE * sizeof(char));
            idxBuf = 0;
            
            // Print the entropy distribution;
            iCountBatch = 0;
            for (j = 0 ; j < pEntropyInfo->ulSizeArray ; j++) {
                snprintf(bufBig + idxBuf, BUF_SIZE_LARGE, "\tBlk #%d: %.3lf\n", j, pEntropyInfo->arrEntropy[j]);
                idxBuf = strlen(bufBig);
                
                iCountBatch++;
                if (iCountBatch == BATCH_WRITE_LINE_COUNT) {
                    Fwrite(bufBig, sizeof(char), idxBuf, fpOutput);
                    memset(bufBig, 0, BUF_SIZE_LARGE * sizeof(char));
                    idxBuf = 0;
                    iCountBatch = 0;
                }
            }
            Fwrite(bufBig, sizeof(char), idxBuf, fpOutput);
            memset(bufBig, 0, BUF_SIZE_LARGE * sizeof(char));
            idxBuf = 0;
        }
    } catch(EXCEPT_IO_FILE_OPEN) {
        iRet = -1;
    } catch(EXCEPT_IO_FILE_WRITE) {
        iRet = -1;
    }
    end_try;    

    if(fpOutput != NULL)
        Fclose(fpOutput);
    
    return iRet;
}

int LogNGramModel(PEInfo *pPEInfo, const char *cszOutputFolder) {
    int              i, iRet, iLenFolder, idxBuf, iCountBatch;
    char             bufMid[BUF_SIZE_MID], bufBig[BUF_SIZE_LARGE];
    FILE             *fpOutput;
    NGramSingleSlice *arrSlice;
    
    iRet = 0;
    
    if (pPEInfo->pNGramModel == NULL)
        goto EXIT;
    
    // Generate the path string of the output text file.
    memset(bufMid, 0, BUF_SIZE_MID * sizeof(char));
    iLenFolder = strlen(cszOutputFolder);
    if (cszOutputFolder[iLenFolder - 1] == OS_PATH_SEPARATOR)
        snprintf(bufMid, BUF_SIZE_MID, "%s%s", cszOutputFolder, 
                                               REPORT_NAME_TXT_NGRAM_MODEL);
    else
        snprintf(bufMid, BUF_SIZE_MID, "%s%c%s", cszOutputFolder,
                                                 OS_PATH_SEPARATOR,
                                                 REPORT_NAME_TXT_NGRAM_MODEL); 
    // Print the n-gram model.
    try {
        fpOutput = NULL;
        fpOutput = Fopen(bufMid, "w");
        
        memset(bufBig, 0, BUF_SIZE_LARGE * sizeof(char));
        arrSlice = pPEInfo->pNGramModel->arrSlice;
        idxBuf = 0;
        iCountBatch = 0;
        for(i = 0 ; i < pPEInfo->pNGramModel->ulModelRealSize ; i++) {
            snprintf(bufBig + idxBuf, BUF_SIZE_LARGE, "%d\t%.3lf\t#(0x%08x:%d)\t(0x%08x:%d)\n",
                    i, arrSlice[i].dFactor, 
                    arrSlice[i].tokenNumerator.ulValue,
                    arrSlice[i].tokenNumerator.ulFrequency,
                    arrSlice[i].tokenDenominator.ulValue,
                    arrSlice[i].tokenDenominator.ulFrequency);
            idxBuf = strlen(bufBig);
                    
            iCountBatch++;
            if(iCountBatch == BATCH_WRITE_LINE_COUNT) {
                Fwrite(bufBig, sizeof(char), idxBuf, fpOutput);
                memset(bufBig, 0, BUF_SIZE_LARGE * sizeof(char));
                idxBuf = 0;
                iCountBatch = 0;
            }
        }
        Fwrite(bufBig, sizeof(char), idxBuf, fpOutput);
    } catch(EXCEPT_IO_FILE_OPEN) {
        iRet = -1;
    } catch(EXCEPT_IO_FILE_WRITE) {
        iRet = -1;
    } end_try;

    if(fpOutput != NULL)
        Fclose(fpOutput);

EXIT:        
    return iRet;
}

int PlotNGramModel(PEInfo *pPEInfo, const char *cszOutputFolder) {
    int         i, j, iRet, iLenFolder, idxBuf, idxPath, iCountBatch;
    char        bufPathScript[BUF_SIZE_MID], bufMid[BUF_SIZE_MID], bufBig[BUF_SIZE_LARGE];
    HINSTANCE   hExec;
    FILE        *fpOutput;

    iRet = 0;    
    
    if (pPEInfo->pNGramModel == NULL)
        goto EXIT;
    
    //---------------------------------------------------------------------------
    // First phase: Generate the gnuplot script to specify the drawing commands.
    //---------------------------------------------------------------------------
    // Generate the path string for the gnuplot script.
    memset(bufPathScript, 0, BUF_SIZE_MID * sizeof(char));
    iLenFolder = strlen(cszOutputFolder);
    if (cszOutputFolder[iLenFolder - 1] == OS_PATH_SEPARATOR)
        snprintf(bufPathScript, BUF_SIZE_MID, "%s%s", cszOutputFolder, 
                                               REPORT_NAME_GNU_PLOT_SCRIPT);
    else
        snprintf(bufPathScript, BUF_SIZE_MID, "%s%c%s", cszOutputFolder,
                                              OS_PATH_SEPARATOR,
                                              REPORT_NAME_GNU_PLOT_SCRIPT);
    try {
        fpOutput = NULL;
        fpOutput = Fopen(bufPathScript, "w");

        // Print the plotting commands.
        memset(bufBig, 0, BUF_SIZE_LARGE * sizeof(char));
        
        snprintf(bufBig, BUF_SIZE_LARGE, "set terminal png size %d, %d\n", 
                REPORT_IMAGE_SIZE_WIDTH, REPORT_IMAGE_SIZE_HEIGHT);
        idxBuf = strlen(bufBig);
    
        // For the path string of the n-gram model image.
        memset(bufMid, 0, BUF_SIZE_MID * sizeof(char));
        idxPath = 0;
        for (i = 0 ; i < iLenFolder ; i++) {
            if (cszOutputFolder[i] == OS_PATH_SEPARATOR) {
                bufMid[idxPath++] = OS_PATH_SEPARATOR;
                #ifdef _WIN32
                    bufMid[idxPath++] = OS_PATH_SEPARATOR;
                #endif
            } else 
                bufMid[idxPath++] = cszOutputFolder[i];
        }
        if (cszOutputFolder[iLenFolder - 1] == OS_PATH_SEPARATOR)
            snprintf(bufMid + idxPath, BUF_SIZE_MID, "%s.png", pPEInfo->szSample);
        else {
            #ifdef _WIN32
                snprintf(bufMid + idxPath, BUF_SIZE_MID, "%c%c%s.png", OS_PATH_SEPARATOR, OS_PATH_SEPARATOR, pPEInfo->szSample);
            #else
                snprintf(bufMid + idxPath, BUF_SIZE_MID, "%c%s.png", OS_PATH_SEPARATOR, pPEInfo->szSample);
            #endif
        }
        snprintf(bufBig + idxBuf, BUF_SIZE_LARGE, "set output \"%s\"\n", bufMid);
        idxBuf = strlen(bufBig);
    
        snprintf(bufBig + idxBuf, BUF_SIZE_LARGE, "set title \"%s\"\n", pPEInfo->szSample);
        idxBuf = strlen(bufBig);
        
        snprintf(bufBig + idxBuf, BUF_SIZE_LARGE, "set xlabel \"Rate\"\n");
        idxBuf = strlen(bufBig);
        
        snprintf(bufBig + idxBuf, BUF_SIZE_LARGE, "set ylabel \"Token\"\n");
        idxBuf = strlen(bufBig);
            
        // For the path string of the n-gram model text dump.
        memset(bufMid, 0, BUF_SIZE_MID * sizeof(char));
        idxPath = 0;
        for (i = 0 ; i < iLenFolder ; i++) {
            if (cszOutputFolder[i] == OS_PATH_SEPARATOR) {
                bufMid[idxPath++] = OS_PATH_SEPARATOR;
                #ifdef _WIN32
                    bufMid[idxPath++] = OS_PATH_SEPARATOR;
                #endif
            } else
                bufMid[idxPath++] = cszOutputFolder[i];
        }
        if (cszOutputFolder[iLenFolder - 1] == OS_PATH_SEPARATOR)
            snprintf(bufMid + idxPath, BUF_SIZE_MID, "%s", REPORT_NAME_TXT_NGRAM_MODEL);
        else {
            #ifdef _WIN32
                snprintf(bufMid + idxPath, BUF_SIZE_MID, "%c%c%s", OS_PATH_SEPARATOR, OS_PATH_SEPARATOR, REPORT_NAME_TXT_NGRAM_MODEL);
            #else
                snprintf(bufMid + idxPath, BUF_SIZE_MID, "%c%s", OS_PATH_SEPARATOR, REPORT_NAME_TXT_NGRAM_MODEL);
            #endif
        }
        snprintf(bufBig + idxBuf, BUF_SIZE_LARGE, "plot \"%s\" title \"\" with lines\n", bufMid);
        idxBuf = strlen(bufBig);
    
        snprintf(bufBig + idxBuf, BUF_SIZE_LARGE, "exit\n", REPORT_NAME_TXT_NGRAM_MODEL);
        idxBuf = strlen(bufBig);

        Fwrite(bufBig, sizeof(char), idxBuf, fpOutput);
    } catch(EXCEPT_IO_FILE_OPEN) {
        iRet = -1;
        goto EXIT;
    } catch(EXCEPT_IO_FILE_WRITE) {
        iRet = -1;
        goto CLOSE_FILE;
    } end_try;
 
    //---------------------------------------------------------------------------
    // Second phase: Plot the image using gnuplot utility.
    //---------------------------------------------------------------------------
    memset(bufMid, 0, sizeof(char) * BUF_SIZE_MID);
    snprintf(bufMid, BUF_SIZE_MID, "\"%s\"", bufPathScript);
    hExec = ShellExecute(NULL, NULL, "gnuplot.exe", bufMid, NULL, SW_SHOWNA);
    if((int)hExec <= 32) {
        Log1("Can not launch the gnuplot utility. (Error Code: %d)\n", GetLastError());
        iRet = -1;
    }
CLOSE_FILE:
    if (fpOutput != NULL)
        Fclose(fpOutput);
EXIT:
    return iRet;
}