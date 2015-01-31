#include "util.h"
#include "except.h"
#include "pe_info.h"
#include "region.h"
#include "ngram.h"
#include "report.h"

/* Print the program usage message. */
void print_usage();


/* Bundle operations to manipulate PEInfo structure and parse input sample. */
int parse_pe_info(PEInfo **ppPEInfo, const char *cszInput);


/* Bundle operations to manipulate RegionCollector structure and select the user-specified features. */
int select_features(RegionCollector **ppRegionCollector, const char *cszLibName, PEInfo *pPEInfo);


/* Bundle operations to manipulate NGram structure and generate the user-specified model. */
int generate_model(NGram **ppNGram, const char *cszLibName, uchar ucDimension, 
                    PEInfo *pPEInfo, RegionCollector *pRegionCollector);


/* Bundle operations to manipulate Report structure and generate the relevant reports. */
int generate_report(Report **ppReport, PEInfo *pPEInfo, NGram *pNGram, const char *cszOutDir, uint uiMask);


int main(int argc, char **argv, char **envp) {
    int             opt, rc, idxOpt, i, iLen;
    uint            uiMask;
    uchar           ucDimension;
    const char      *cszInput, *cszOutput, *cszReportSeries, *cszLibRegion, *cszLibModel;
    PEInfo          *pPEInfo;
    RegionCollector *pRegionCollector;
    NGram           *pNGram;
    Report          *pReport;
    char            szOrder[BUF_SIZE_SMALL];
    
    /* Craft the structure to store command line options. */    
    static struct option Options[] = {
        {OPT_LONG_HELP     , required_argument, 0, OPT_HELP     },
        {OPT_LONG_INPUT    , required_argument, 0, OPT_INPUT    },
        {OPT_LONG_OUTPUT   , required_argument, 0, OPT_OUTPUT   },
        {OPT_LONG_DIMENSION, required_argument, 0, OPT_DIMENSION},
        {OPT_LONG_REPORT   , required_argument, 0, OPT_REPORT   },
        {OPT_LONG_REGION   , required_argument, 0, OPT_REGION   },
        {OPT_LONG_MODEL    , required_argument, 0, OPT_MODEL    },
    };

    memset(szOrder, 0, sizeof(char) * BUF_SIZE_SMALL);
    sprintf(szOrder, "%c%c:%c:%c:%c:%c:%c:", OPT_HELP, OPT_INPUT, OPT_OUTPUT, OPT_DIMENSION, 
                                             OPT_REPORT, OPT_REGION, OPT_MODEL);    
    cszInput = cszOutput = cszReportSeries = cszLibRegion = cszLibModel = NULL;
    rc = 0;

    /* Get the command line options. */
    idxOpt = 0;
    while ((opt = getopt_long(argc, argv, szOrder, Options, &idxOpt)) != -1) {
        switch (opt) {
            case OPT_INPUT: {
                cszInput = optarg;
                break;
            }
            case OPT_OUTPUT: {
                cszOutput = optarg;
                break;
            }
            case OPT_REPORT: {
                cszReportSeries = optarg;
                break;
            }
            case OPT_REGION: {
                cszLibRegion = optarg;
                break;
            }
            case OPT_MODEL: {
                cszLibModel = optarg;
                break;
            }
            case OPT_DIMENSION: {
                ucDimension = atoi(optarg);
                break;
            }
            default: {
                print_usage();
                rc = -1;
                goto EXIT;            
            }
        }
    }

    /* Check the length of path string. */
    if ((cszInput == NULL) || (strlen(cszInput) == 0)) {
        print_usage();
        rc = -1;
        goto EXIT;
    }

    if ((cszOutput == NULL) || (strlen(cszOutput) == 0)) {
        print_usage();
        rc = -1;
        goto EXIT;
    }

    /* Check the dimension. */
    if ((ucDimension == 0) || (ucDimension > 4)) {
        print_usage();        
        rc = -1;
        goto EXIT;
    }    

    /* Check the designated report type. */
    if ((cszReportSeries == NULL) || ((iLen = strlen(cszReportSeries)) == 0)) {
        uiMask = MASK_REPORT_SECTION_ENTROPY | MASK_REPORT_TXT_NGRAM | \
                 MASK_REPORT_PNG_NGRAM;
    } else {
        uiMask = 0;
        for (i = 0 ; i < iLen ; i++) {
            switch(cszReportSeries[i]) {
                case ABV_TOKEN_REPORT_SECTION_ENTROPY: {
                    uiMask |= MASK_REPORT_SECTION_ENTROPY;
                    break;                
                }
                case ABV_TOKEN_REPORT_TXT_NGRAM: {
                    uiMask |= MASK_REPORT_TXT_NGRAM;
                    break;
                }
                case ABV_TOKEN_REPORT_PNG_NGRAM: {
                    uiMask |= MASK_REPORT_PNG_NGRAM;
                    break;            
                }
            }
        }
    }
    if (uiMask == 0) {
        print_usage();
        rc = -1;
        goto EXIT;
    }

    /* Prepare the basic PE features. */
    rc = parse_pe_info(&pPEInfo, cszInput);
    if (rc != 0)
        goto FREE_PE;

    /* Select the features for n-gram model generation. */
    rc = select_features(&pRegionCollector, cszLibRegion, pPEInfo);
    if (rc != 0)
        goto FREE_RC;

    /* Generate the model with the selected features. */
    rc = generate_model(&pNGram, cszLibModel, ucDimension, pPEInfo, pRegionCollector);
    if (rc != 0)
        goto FREE_NG;

    /* Generate the relevant reports for the model. */
    rc = generate_report(&pReport, pPEInfo, pNGram, cszOutput, uiMask);

    /* Deinitialize the Report structure*/
    Report_deinit(pReport);

    /* Deinitialize the NGram structure. */
FREE_NG:
    NGram_deinit(pNGram);

    /* Deinitialize the RegionCollector structure. */
FREE_RC:
    RegionCollector_deinit(pRegionCollector);

    /* Deinitialize the PEInfo structure. */
FREE_PE:
    PEInfo_deinit(pPEInfo);

EXIT:
    return rc;
}


void print_usage() {

    const char *cszMsg = "Usage: pe_ngram --input path_input --output path_output --dimension num --report flags.\n"
                         "       pe_ngram -i      path_input -o       path_output -d          num -t       flags.\n\n"
                         "       path_input : The path to the input sample.\n"
                         "       path_output: The path to the output report folder.\n"
                         "                    (Only accpet absolute paths.)\n"
                         "       dimension  : The dimension of n-gram model.\n"
                         "                    (Dimension must be larger than 0 and be less than 5.)\n"
                         "       flags      : The set of control flags for report types.\n"
                         "                    (flag 'e' : For text dump of entropy distribution.)\n"
                         "                    (flag 't' : For text dump of n-gram model.)\n"
                         "                    (flag 'i' : For visualized image of n-gram model.)\n"
                         "                    (The 'i' flag must be after the 't' flag.)\n"
                         "                    (e.g. : e, t, i, et, eti)\n\n"
                         "Example: pe_ngram --input /repo/sample/a.exe --output /repo/analysis/a --dimension 2 --report eti\n"
                         "         pe_ngram -i /repo/sample/a.exe -o /repo/sample/a -d 2 -t eti\n\n";
    printf("%s", cszMsg);
    return;
}


int parse_pe_info(PEInfo **ppPEInfo, const char *cszInput) {
    int     rc;
    PEInfo  *pPEInfo;    

    rc = 0;

    /* Initialize the PEInfo structure. */
    PEInfo_init(*ppPEInfo);
    if (*ppPEInfo == NULL) {
        rc = -1;
        goto EXIT;
    }

    pPEInfo = *ppPEInfo;        

    /* Open the input sample for analysis. */
    rc = pPEInfo->openSample(pPEInfo, cszInput);
    if (rc != 0)
        goto EXIT;

    /* Collect the header information of the input sample. */
    rc = pPEInfo->parseHeaders(pPEInfo);
    if (rc != 0)
        goto EXIT;

    /* Calculate and collect entropy data for each section. */
    rc = pPEInfo->calculateSectionEntropy(pPEInfo);
    if (rc != 0)
        goto EXIT;

EXIT:
    return rc;
}


int select_features(RegionCollector **ppRegionCollector, const char *cszLibName, PEInfo *pPEInfo) {
    int     rc;
    RegionCollector *pRegionCollector;

    rc = 0;    

    /* Initialize the RegionCollector structure. */
    RegionCollector_init(*ppRegionCollector);
    if (*ppRegionCollector != NULL) {
        pRegionCollector = *ppRegionCollector;

        /* Select the features. */
        rc = pRegionCollector->selectFeatures(pRegionCollector, cszLibName, pPEInfo);
    } else
        rc = -1;

    return rc;
}


int generate_model(NGram **ppNGram, const char *cszLibName, uchar ucDimension, 
                    PEInfo *pPEInfo, RegionCollector *pRegionCollector) {
    int     rc;
    NGram   *pNGram;

    rc = 0;

    /* Initialize the NGram structure. */
    NGram_init(*ppNGram);

    if (*ppNGram != NULL) {
        pNGram = *ppNGram;

        /* Set the maximum value of a n-gram token. */
        pNGram->setDimension(pNGram, ucDimension);

        /* Generate the model. */
        rc = pNGram->generateModel(pNGram, cszLibName, pPEInfo, pRegionCollector);
    } else
        rc = -1;

    return rc;
}


int generate_report(Report **ppReport, PEInfo *pPEInfo, NGram *pNGram, const char *cszOutDir, uint uiMask) {
    int        rc;
    const char *cszSampleName;
    Report     *pReport;

    rc = 0;
    
    /* Initialize the Report structure. */
    Report_init(*ppReport);

    if (*ppReport != NULL) {
        pReport = *ppReport;

        /* Retrieve the sample name. */
        cszSampleName = pPEInfo->szSampleName;

        /* Generate the report folder. */
        rc = pReport->generateFolder(pReport, cszOutDir);
        if (rc != 0)
            goto EXIT;

        /* Generate the entropy distribution report. */
        if (uiMask & MASK_REPORT_SECTION_ENTROPY) {
            rc = pReport->logEntropyDistribution(pReport, pPEInfo, cszOutDir, cszSampleName);
            if (rc != 0)
                goto EXIT;
        }

        /* Generate the full n-gram model report. */
        if (uiMask & MASK_REPORT_TXT_NGRAM) {        
            rc = pReport->logNGramModel(pReport, pNGram, cszOutDir, cszSampleName);
            if (rc != 0)
                goto EXIT;
        }

        /* Generate the visualized n-gram model. */
        if (uiMask & MASK_REPORT_PNG_NGRAM) {
            rc = pReport->plotNGramModel(pReport, pNGram, cszOutDir, cszSampleName);
            if (rc != 0)
                goto EXIT;
        }
    } else 
        rc = -1;

EXIT:
    return rc;
}

