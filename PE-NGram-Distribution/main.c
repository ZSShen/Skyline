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
int select_features(RegionCollector **ppRegionCollector, const char *cszMethod, PEInfo *pPEInfo);


/* Bundle operations to manipulate NGram structure and generate the user-specified model. */
int generate_model(NGram **ppNGram, const char *cszMethod, uchar ucDimension, 
                    PEInfo *pPEInfo, RegionCollector *pRegionCollector);


/* Bundle operations to manipulate Report structure and generate the relevant reports. */
int generate_report(Report **ppReport, PEInfo *pPEInfo, const char *cszOutDir);


int main(int argc, char **argv) {
    int             opt, rc, idxOpt;
    uchar           ucDimension;
    const char      *cszInput, *cszOutput;
    PEInfo          *pPEInfo;
    RegionCollector *pRegionCollector;
    NGram           *pNGram;
    Report          *pReport;
    
    /* Craft the structure to store command line options. */    
    static struct option Options[] = {
        {"input"    , required_argument, 0, 'i'},
        {"output"   , required_argument, 0, 'o'},
        {"dimension", required_argument, 0, 'd'}
    };
    const char *cszOrder = "i:o:d:";

    cszInput = cszOutput = NULL;
    rc = 0;

    /* Get the command line options. */
    idxOpt = 0;
    while ((opt = getopt_long(argc, argv, cszOrder, Options, &idxOpt)) != -1) {
        switch (opt) {
            case 'i': {
                cszInput = optarg;
                break;
            }
            case 'o': {
                cszOutput = optarg;
                break;
            }
            case 'd': {
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

    /* Prepare the basic PE features. */
    rc = parse_pe_info(&pPEInfo, cszInput);
    if (rc != 0)
        goto FREE_PE;

    /* Select the features for n-gram model generation. */
    rc = select_features(&pRegionCollector, NULL, pPEInfo);
    if (rc != 0)
        goto FREE_RC;

    /* Generate the model with the selected features. */
    rc = generate_model(&pNGram, NULL, ucDimension, pPEInfo, pRegionCollector);
    if (rc != 0)
        goto FREE_NG;

    /* Generate the relevant reports for the model. */
    rc = generate_report(&pReport, pPEInfo, cszOutput);


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

    const char *cszMsg = "Usage: ngram_distribution -input path_input -output path_output -dimension num.\n"
                         "       path_input : The path to the input sample.\n"
                         "       path_output: The path to the output report folder.\n"
                         "       dimension  : The dimension of n-gram model.\n"
                         "                    (Must be larger than 0 and be less than 5.)\n";   
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


int select_features(RegionCollector **ppRegionCollector, const char *cszMethod, PEInfo *pPEInfo) {
    int     rc;
    RegionCollector *pRegionCollector;

    rc = 0;    

    /* Initialize the RegionCollector structure. */
    RegionCollector_init(*ppRegionCollector);
    if (*ppRegionCollector != NULL) {
        pRegionCollector = *ppRegionCollector;

        /* Select the features. */
        rc = pRegionCollector->selectFeatures(pRegionCollector, cszMethod, pPEInfo);
    } else
        rc = -1;

    return rc;
}


int generate_model(NGram **ppNGram, const char *cszMethod, uchar ucDimension, 
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
        rc = pNGram->generateModel(pNGram, cszMethod, pPEInfo, pRegionCollector);
    } else
        rc = -1;

    return rc;
}


int generate_report(Report **ppReport, PEInfo *pPEInfo, const char *cszOutDir) {
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

        /* Generate the entropy distribution report. */
        rc = pReport->logEntropyDistribution(pReport, cszOutDir, cszSampleName);
    } else 
        rc = -1;

    return rc;
}

