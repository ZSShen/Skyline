#include "region.h"

/*===========================================================================*
 *                  Definition for internal functions                        *
 *===========================================================================*/

/**
 * This function selects the binary regions from the entire section with maximum average entropy.
 * 
 * @param   self        The pointer to the RegionCollector structure.
 * @param   pPEInfo     The pointer to the to be analyzed PEInfo structure.
 *
 * @return              0: The binary regions are collected successfully.
 *                    < 0: Exception occurs while memory allocation.
 */
int _FuncEntirePlanInMaxSec(RegionCollector *self, PEInfo *pPEInfo);


/**
 * This function selects the binary regions which have the entropy values higher than the threshold
 * from the section with maximum average entropy.
 *
 * @param   self        The pointer to the RegionCollector structure.
 * @param   pPEInfo     The pointer to the to be analyzed PEInfo structure.
 *
 * @return              0: The binary regions are collected successfully.
 *                    < 0: Exception occurs while memory allocation.
 */
int _FuncPlateausInMaxSec(RegionCollector *self, PEInfo *pPEInfo);


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
    int rc;
    
    /* The default method is _FuncEntirePlanInMaxSec(). */
    if (cszMethod == NULL)
        rc = _FuncEntirePlanInMaxSec(self, pPEInfo);


    return 0;
}


/*===========================================================================*
 *                Implementation for internal functions                      *
 *===========================================================================*/

int _FuncEntirePlanInMaxSec(RegionCollector *self, PEInfo *pPEInfo) {


    return 0;
}
