#include "region.h"


void RCInit(RegionCollector *self) {

    self->usNumRegions = 0;
    self->arrRegion = NULL;

    self->selectFeatures = RCSelectFeatures;

    return;
}


void RCDeinit(RegionCollector *self) {
    int       i;
    Region    *pRegion;
    RangePair *listPair, *pPair;

    if (self->arrRegion != NULL) {
        for (i = 0 ; i < self->usNumRegions ; i++) {
        
            /* Free the Range structure. */            
            pRegion = self->arrRegion[i];
            if (pRegion != NULL) {

                /* Free the list of RangePair structures. */
                listPair = pRegion->listRangePair;
                while (listPair != NULL) {
                    pPair = listPair->next;
                    Free(pPair);
                    listPair = pPair;
                } 
            }
        }
    }

    return;
}


/**
 * RCSelectFeatures() Select the features for n-gram model generation based on the specified method.
 */
int RCSelectFeatures(RegionCollector *self, const char *cszMethod, PEInfo *pPEInfo) {

    return 0;
}
