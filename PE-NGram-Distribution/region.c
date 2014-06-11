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
    int         rc, i;
    ushort      usNumRegions, usIdxSection;
    double      dMax;
    SectionInfo *pSection;
    Region      *pRegion;

    rc = 0;
    try {
        /* Only the section with maximum average entropy will be selected. */
        self->usNumRegions = 1;
        self->arrRegion = (Region**)Malloc(sizeof(Region*));
        self->arrRegion[0] = NULL;

        dMax = -1;
        for (i = 0 ; i < pPEInfo->pPEHeader->usNumSections ; i++) {
            pSection = pPEInfo->arrSectionInfo[i];            
            if ((pSection != NULL) && (pSection->ulRawSize != 0)) {
                if (dMax < pSection->pEntropyInfo->dAvgEntropy) {
                    dMax = pSection->pEntropyInfo->dAvgEntropy;
                    usIdxSection = i;
                }
            }
        }

        /* Record the region information. */
        self->arrRegion[0] = (Region*)Malloc(sizeof(Region));
        pRegion = self->arrRegion[0];
        pRegion->usIdxSection = usIdxSection;
        pRegion->arrRangePair = NULL;

        pRegion->arrRangePair = (RangePair**)Malloc(sizeof(RangePair*));
        pRegion->ulNumPairs = 0;
        pRegion->arrRangePair[0] = NULL;

        pRegion->arrRangePair[0] = (RangePair*)Malloc(sizeof(RangePair));
        pRegion->ulNumPairs = 1;        
        pRegion->arrRangePair[0]->ulIdxBgn = 0;
        pRegion->arrRangePair[0]->ulIdxEnd = pPEInfo->arrSectionInfo[usIdxSection]->pEntropyInfo->ulNumBlks;
    } catch(EXCEPT_MEM_ALLOC) {
        rc = -1;
    } end_try;

    return rc;
}
