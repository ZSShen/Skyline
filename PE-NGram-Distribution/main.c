#include "util.h"
#include "except.h"
#include "pe_info.h"


int main(int argc, char **argv) {
    int         rc;    
    const char  *cszInput;
    PEInfo      *pPEInfo;

    rc = 0;
    cszInput = argv[1];    

    /* Initialize the PEInfo structure. */
    PEInfo_init(pPEInfo);

    /* Open the input sample for analysis */
    pPEInfo->openSample(pPEInfo, cszInput);

    /* Deinitialize the PEInfo structure. */
    PEInfo_deinit(pPEInfo);

    return rc;
}
