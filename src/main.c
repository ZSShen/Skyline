#include "util.h"
#include "except.h"
#include "pe_info.h"
#include "region.h"
#include "ngram.h"
#include "report.h"


typedef struct _Opt {
    const char *cszInput;
    const char *cszOutput;
    const char *cszLibRegion;
    const char *cszLibModel;
    uchar ucDimension;
} Opt;


/* Print the program usage message. */
void print_usage();

/* Initialize the primary worker modules. */
int init_modules(PEInfo**, RegionCollector**, NGram**, Report**, Opt*);

/* Deinitialize the primary worker modules. */
int deinit_modules(PEInfo*, RegionCollector*, NGram*, Report*);

/* Bundle the operations to parse input PE file. */
int parse_pe_info(PEInfo*, const char*);

/* Bundle the operations to select the user-specified binary characteristics. */
int select_features(RegionCollector*, PEInfo*);

/* Bundle the operations to generate the user-specified model. */
int generate_model(NGram*, uchar, PEInfo*, RegionCollector*);

/* Bundle the operations to generate the reports. */
int generate_report(Report*, PEInfo*, NGram*, const char*, uint);


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
    Opt             bundleOpt;

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

    bundleOpt.ucDimension = ucDimension;
    bundleOpt.cszInput = cszInput;
    bundleOpt.cszOutput = cszOutput;
    bundleOpt.cszLibRegion = cszLibRegion;
    bundleOpt.cszLibModel = cszLibModel;

    rc = init_modules(&pPEInfo, &pRegionCollector, &pNGram, &pReport, &bundleOpt);
    if (rc != 0)
        goto DEINIT;

    /* Prepare the basic PE features. */
    rc = parse_pe_info(pPEInfo, cszInput);
    if (rc != 0)
        goto DEINIT;

    /* Select the features for n-gram model generation. */
    rc = select_features(pRegionCollector, pPEInfo);
    if (rc != 0)
        goto DEINIT;

    /* Generate the model with the selected features. */
    rc = generate_model(pNGram, ucDimension, pPEInfo, pRegionCollector);
    if (rc != 0)
        goto DEINIT;

    /* Generate the relevant reports for the model. */
    rc = generate_report(pReport, pPEInfo, pNGram, cszOutput, uiMask);

DEINIT:
    deinit_modules(pPEInfo, pRegionCollector, pNGram, pReport);

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


int init_modules(PEInfo **ppPEInfo, RegionCollector **ppRegionCollector,
                 NGram **ppNGram, Report **ppReport, Opt *pOpt) {
    int rc;

    rc = 0;
    PEInfo_init(*ppPEInfo);
    if (*ppPEInfo == NULL) {
        rc = -1;
        goto EXIT;
    }
    RegionCollector_init(*ppRegionCollector);
    if (*ppRegionCollector == NULL) {
        rc = -1;
        goto EXIT;
    }
    NGram_init(*ppNGram);
    if (*ppNGram == NULL) {
        rc = -1;
        goto EXIT;
    }
    Report_init(*ppReport);
    if (*ppReport == NULL) {
        rc = -1;
        goto EXIT;
    }

    rc = (*ppRegionCollector)->loadPlugin(*ppRegionCollector, pOpt->cszLibRegion);
    if (rc != 0)
        goto EXIT;
    rc = (*ppNGram)->loadPlugin(*ppNGram, pOpt->cszLibModel);
    if (rc != 0)
        goto EXIT;
    rc = (*ppReport)->generateFolder(*ppReport, pOpt->cszOutput);

EXIT:
    return rc;
}


int deinit_modules(PEInfo *pPEInfo, RegionCollector *pRegionCollector,
                   NGram *pNGram, Report *pReport) {
    if (pPEInfo != NULL)
        PEInfo_deinit(pPEInfo);
    if (pRegionCollector != NULL) {
        pRegionCollector->unloadPlugin(pRegionCollector);
        RegionCollector_deinit(pRegionCollector);
    }
    if (pNGram != NULL) {
        pNGram->unloadPlugin(pNGram);
        NGram_deinit(pNGram);
    }
    if (pReport != NULL)
        Report_deinit(pReport);
    return 0;
}


int parse_pe_info(PEInfo *pPEInfo, const char *cszInput) {
    int rc;

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


int select_features(RegionCollector *pRegionCollector, PEInfo *pPEInfo) {
    return pRegionCollector->selectFeatures(pRegionCollector, pPEInfo);
}


int generate_model(NGram *pNGram, uchar ucDimension, PEInfo *pPEInfo,
                   RegionCollector *pRegionCollector) {
    /* Set the maximum value of a n-gram token. */
    pNGram->setDimension(pNGram, ucDimension);
    /* Generate the model. */
    return pNGram->generateModel(pNGram, pPEInfo, pRegionCollector);
}


int generate_report(Report *pReport, PEInfo *pPEInfo, NGram *pNGram, const char *cszOutDir, uint uiMask) {
    int rc;
    const char *cszSampleName;

    /* Retrieve the sample name. */
    cszSampleName = pPEInfo->szSampleName;

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

EXIT:
    return rc;
}

