#include "util.h"
#include "except.h"
#include "pe_info.h"
#include "region.h"


int parse_pe_info(PEInfo **ppPEInfo, const char *cszInput);

int select_features(RegionCollector **ppRegionCollector, const char *cszMethod, PEInfo *pPEInfo);


int main(int argc, char **argv) {
    int             rc;    
    const char      *cszInput;
    PEInfo          *pPEInfo;
    RegionCollector *pRegionCollector;

    rc = 0;
    cszInput = argv[1];    

    /* Prepare the basic PE features. */
    rc = parse_pe_info(&pPEInfo, cszInput);
    if (rc != 0)
        goto FREE_PE;

    /* Select the features for n-gram model generation. */
    rc = select_features(&pRegionCollector, NULL, pPEInfo);
    if (rc != 0)
        goto FREE_RC;


    /* Deinitialize the RegionCollector structure. */
FREE_RC:
    RegionCollector_deinit(pRegionCollector);

    /* Deinitialize the PEInfo structure. */
FREE_PE:
    PEInfo_deinit(pPEInfo);

    return rc;
}


int parse_pe_info(PEInfo **ppPEInfo, const char *cszInput) {
    int     rc;
    PEInfo  *pPEInfo;    

    rc = 0;

    /* Initialize the PEInfo structure. */
    PEInfo_init(*ppPEInfo);
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
    pRegionCollector = *ppRegionCollector;

    return rc;
}
