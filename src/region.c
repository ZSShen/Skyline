#include "region.h"


/*===========================================================================*
 *                Implementation for exported functions                      *
 *===========================================================================*/
void RCInit(RegionCollector *self) {
    /* Initialize member variables. */
    self->usNumRegions = 0;
    self->arrRegion = NULL;
    self->hdlePlug = NULL;
    self->entryPlug = NULL;

    /* Assign the default member functions. */
    self->loadPlugin = RCLoadPlugin;
    self->unloadPlugin = RCUnloadPlugin;
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

int RCLoadPlugin(RegionCollector *self, const char *cszName) {
    int rc;
    char szLib[BUF_SIZE_SMALL];

    rc = 0;
    try {
        memset(szLib, 0, sizeof(char) * BUF_SIZE_SMALL);
        if (cszName == NULL)
            sprintf(szLib, "lib%s.so", LIB_DEFAULT_MAX_ENTROPY_SEC);
        else
            sprintf(szLib, "lib%s.so", cszName);

        self->hdlePlug = Dlopen(szLib, RTLD_LAZY);
        self->entryPlug = Dlsym(self->hdlePlug, PLUGIN_ENTRY_REGION);
    } catch(EXCEPT_DL_LOAD) {
        rc = -1;
    } catch(EXCEPT_DL_GET_SYMBOL) {
        rc = -1;
    } end_try;

    return rc;
}

int RCUnloadPlugin(RegionCollector *self) {
    if (self->hdlePlug != NULL)
        Dlclose(self->hdlePlug);
    return 0;
}

int RCSelectFeatures(RegionCollector *self, PEInfo *pPEInfo) {
    return self->entryPlug(self, pPEInfo);
}
