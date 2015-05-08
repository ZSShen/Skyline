#ifndef _REGION_H_
#define _REGION_H_

#include "util.h"
#include "except.h"
#include "pe_info.h"


/* Structure to record the range of binary. */
typedef struct _RangePair {
    ulong ulIdxBgn, ulIdxEnd;
} RangePair;


/* Structure to the features collected within a section. */
typedef struct _Region {
    ushort      usIdxSection;
    ulong       ulNumPairs;
    RangePair   **arrRangePair;
} Region;


/* Structure to store the features for n-gram analysis. */
typedef struct _RegionCollector {
    ushort  usNumRegions;
    Region  **arrRegion;

    void *hdlePlug;
    int (*entryPlug) (struct _RegionCollector*, PEInfo*);

    int (*selectFeatures) (struct _RegionCollector*, PEInfo*);
    int (*loadPlugin) (struct _RegionCollector*, const char*);
    int (*unloadPlugin) (struct _RegionCollector*);
} RegionCollector;


/* Wrapper for RegionCollector initialization. */
#define RegionCollector_init(p)     try {                                                       \
                                        p = (RegionCollector*)Malloc(sizeof(RegionCollector));  \
                                        RCInit(p);                                              \
                                    } catch (EXCEPT_MEM_ALLOC) {                                \
                                        p = NULL;                                               \
                                    } end_try;


/* Wrapper for RegionCollector deinitialization. */
#define RegionCollector_deinit(p)   if (p != NULL) {                            \
                                        RCDeinit(p);                            \
                                        Free(p);                                \
                                        p = NULL;                               \
                                    }


/* Constructor for RegionCollector structure. */
void RCInit(RegionCollector *self);


/* Destructor for RegionCollector structure. */
void RCDeinit(RegionCollector *self);


/**
 * This function loads the region collector plugin.
 *
 * @param   self        The pointer to the RegionCollector structure.
 * @param   cszName     The plugin name.
 *
 * @return              0: The plugin is loaded successfully.
 *                    < 0: The plugin cannot be loaded.
 */
int RCLoadPlugin(RegionCollector *self, const char *cszName);


/**
 * This function unloads the region collector plugin.
 *
 * @param   self        The pointer to the RegionCollector structure.
 * @return              0: The plugin is unloaded successfully.
 *                    < 0: The plugin cannot be unloaded.
 */
int RCUnloadPlugin(RegionCollector *self);


/**
 * This function selects the features for n-gram model generation based on the specified method.
 *
 * @param   self        The pointer to the RegionCollector structure.
 * @param   pPEInfo     The pointer to the to be analyzed PEInfo structure.
 *
 * @return              0: The features are collected successfully.
 *                    < 0: Exception occurs while memory allocation.
 */
int RCSelectFeatures(RegionCollector *self, PEInfo *pPEInfo);


#endif
