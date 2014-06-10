#include "util.h"
#include "except.h"
#include "pe_info.h"


int parse_pe_info(PEInfo **ppPEInfo, const char *cszInput);


int main(int argc, char **argv) {
    int         rc;    
    const char  *cszInput;
    PEInfo      *pPEInfo;

    rc = 0;
    cszInput = argv[1];    

    /* Prepare the basic PE features. */
    rc = parse_pe_info(&pPEInfo, cszInput);
    if (rc != 0)
        goto FREE_PE;

    /* Select the features for n-gram model generation. */

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
