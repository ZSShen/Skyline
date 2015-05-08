#include "region.h"

/*---------------------------------------------------------------------------*
 *                          Plugin Objective                                 *
 *                                                                           *
 *  This plugin collects the binary blocks having the entropy values higher  *
 *  than the predefined threshold from the section with maximum average      *
 *  entropy.                                                                 *
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
	return 0;
}

