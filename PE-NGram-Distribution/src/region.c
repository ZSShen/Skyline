#include "region.h"


/*===========================================================================*
 *                Implementation for exported functions                      *
 *===========================================================================*/
void RCInit(RegionCollector *self) {

    /* Initialize member variables. */
    self->usNumRegions = 0;
    self->arrRegion = NULL;

    /* Assign the default member functions. */
    self->selectFeatures = RCSelectFeatures;

    return;
}


void RCDeinit(RegionCollector *self) {
    int     i, j;
    Region  *pRegion;

    if (self->arrRegion != NULL) {
        for (i = 0 ; i < self->usNumRegions ; i++) {
        
            /* Free the Range structure. */            
            pRegion = self->arrRegion[i];
            if (pRegion != NULL) {

                /* Free the array of RangePair structures. */
                if (pRegion->arrRangePair != NULL) {
                    for (j = 0 ; j < pRegion->ulNumPairs ; j++) {
                        if (pRegion->arrRangePair[j] != NULL)
                            Free(pRegion->arrRangePair[j]);                    
                    }
                    Free(pRegion->arrRangePair);
                } 
                Free(pRegion);
            }
        }
        Free(self->arrRegion);
    }

    return;
}


/**
 * RCSelectFeatures() Select the features for n-gram model generation with the specified method.
 */
int RCSelectFeatures(RegionCollector *self, const char *cszLibName, PEInfo *pPEInfo) {
    int     rc;
    void    *hdleLib;
    char    szLib[BUF_SIZE_SMALL];
    FUNC_PTR_REGION funcEntry;
    
    try {
        rc = 0;

        /* The default library is "libRegion_MaxEntropySection.so" . */
        memset(szLib, 0, sizeof(char) * BUF_SIZE_SMALL);        
        if (cszLibName == NULL) 
            sprintf(szLib, "lib%s.so", LIB_DEFAULT_MAX_ENTROPY_SEC);
        else
            sprintf(szLib, "lib%s.so", cszLibName);

        /* Load the plugin. */
        hdleLib = NULL;
        hdleLib = Dlopen(szLib, RTLD_LAZY);

        /* Get the plugin entry point. */
        funcEntry = NULL;
        funcEntry = Dlsym(hdleLib, PLUGIN_ENTRY_POINT);

        /* Run the plugin. */
        rc = funcEntry(self, pPEInfo);
    } catch(EXCEPT_DL_LOAD) {
        rc = -1;
    } catch(EXCEPT_DL_GET_SYMBOL) {
        rc = -1;
    } end_try;

    if (hdleLib != NULL)
        Dlclose(hdleLib);

    return rc;
}

