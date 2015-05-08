#include "region.h"

/*---------------------------------------------------------------------------*
 *                          Plugin Objective                                 *
 *                                                                           *
 * This plugin selects the section with maximum average entropy, and it      *
 * chooses the entire binary of this section for n-gram model generation     *
 *---------------------------------------------------------------------------*/


/*===========================================================================*
 *                Implementation for exported functions                      *
 *===========================================================================*/
/**
 * This function is the entry point of the plugin.
 *
 * @param   pRegionCollector    The pointer to the RegionCollector structure.
 *                              The plugin should put the data into this structure.
 * @param   pPEInfo             The pointer to the PEInfo structure.
 *                              The plugin can refer to this structure to determine
 *                              the binary regions for n-gram generation.
 *
 * @return                      0: The binary regions are collected successfully.
 *                            < 0: Exception occurs while memory allocation.
 */
int region_run(RegionCollector *pRegionCollector, PEInfo *pPEInfo) {
    int         rc, i;
    ushort      usNumRegions, usIdxSection;
    double      dMax;
    SectionInfo *pSection;
    Region      *pRegion;

    rc = 0;
    try {
        /* Select the section with maximum average entropy. */
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
        pRegionCollector->usNumRegions = 0;
        pRegionCollector->arrRegion = NULL;
        pRegionCollector->arrRegion = (Region**)Malloc(sizeof(Region*));
        pRegionCollector->usNumRegions++;

        pRegionCollector->arrRegion[0] = NULL;
        pRegionCollector->arrRegion[0] = (Region*)Malloc(sizeof(Region));
        pRegion = pRegionCollector->arrRegion[0];

        pRegion->usIdxSection = usIdxSection;
        pRegion->ulNumPairs = 0;
        pRegion->arrRangePair = NULL;
        pRegion->arrRangePair = (RangePair**)Malloc(sizeof(RangePair*));
        pRegion->ulNumPairs++;

        pRegion->arrRangePair[0] = NULL;
        pRegion->arrRangePair[0] = (RangePair*)Malloc(sizeof(RangePair));
        pRegion->arrRangePair[0]->ulIdxBgn = 0;
        pRegion->arrRangePair[0]->ulIdxEnd = pPEInfo->arrSectionInfo[usIdxSection]->pEntropyInfo->ulNumBlks;

    } catch(EXCEPT_MEM_ALLOC) {
        rc = -1;
    } end_try;

    return rc;
}

