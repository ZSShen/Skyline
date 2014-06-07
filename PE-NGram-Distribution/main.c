#include "util.h"
#include "except.h"
//#include "pe_info.h"


int main(int argc, char **argv) {
    
    try {
        Fwrite(NULL, 0, 0, NULL);
    } catch(EXCEPT_IO_FILE_OPEN) {

    } end_try;


/*
    int             i, j, iRet, iNGramDimension, iSizeArray;
    const char      *szInput, *szOutput, *cszPolicy;
    PEInfo          *pPEInfo;
    PolicyRegion    *pPolicyRegion;
 
    iRet = 0;
    if(argc < 4) {
        Log0("Usage: main.exe [Path of Input PE] [Path of Output Report Folder] [Dimension N]\n");
        iRet = -1;
        goto EXIT;
    }
    
    szInput = argv[1];
    szOutput = argv[2];
    iNGramDimension = atoi(argv[3]);
    if (argc == 5)
        cszPolicy = argv[4];
    else
        cszPolicy = NULL;
    
    iRet = PEInfoInit(&pPEInfo, szInput);
    if (iRet != 0)
        goto FREE_PEINFO;
        
    iRet = PEInfoParseHeaders(pPEInfo);
    if (iRet != 0)
        goto FREE_PEINFO;

    iRet = PEInfoCalculateSectionEntropy(pPEInfo);
    if (iRet != 0)
        goto FREE_PEINFO;
        
    iRet = PolicySelectBlockRegions(pPEInfo, cszPolicy, &pPolicyRegion);
    if (iRet != 0)
        goto FREE_POLICY_REGION;
    pPolicyRegion->iNGramDimension = iNGramDimension;

    iRet = PolicyGenerateModel(pPEInfo, NULL, pPolicyRegion);
    if (iRet != 0)
        goto FREE_POLICY_REGION;

    iRet = ReportGenerate(pPEInfo, szOutput);
    if (iRet != 0)
        goto FREE_POLICY_REGION;    
        
FREE_POLICY_REGION:
    UninitPolicyRegion(pPolicyRegion);

FREE_PEINFO:    
    PEInfoUninit(pPEInfo);
EXIT:
    return iRet;
*/
}
