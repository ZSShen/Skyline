#include "region.h"

/*---------------------------------------------------------------------------*
 *                          Plugin Objective                                 *
 *                                                                           *
 *  Please briefly describe the features of this plugin.                     *
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
int run(RegionCollector *pRegionCollector, PEInfo *pPEInfo) {
    int rc;
 
    rc = 0;

    return rc;
}

