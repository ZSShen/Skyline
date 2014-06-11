#include "ngram.h"

/*===========================================================================*
 *                  Definition for internal functions                        *
 *===========================================================================*/

/**
 * This function generates the model by sorting the appearance frequencies of n-gram tokens
 * with descending order.
 *
 * @param   self                The pointer to the NGram structure.
 * @param   cszMethod           The string describing the model generation method.
 * @param   pPEInfo             The pointer to the to be analyzed PEInfo structure. 
 * @param   pRegionCollector    The pointer to the RegionCollector structure which stores all the selected features.
 *
 * @return                      0: The model is generated successfully.
 *                            < 0: Exception occurs while memory allocation.
 */
int _FuncTokenFreqDescOrder(NGram *self, const char *cszMethod, PEInfo *pPEInfo, RegionCollector *pRegionCollector);


/*===========================================================================*
 *                Implementation for exported functions                      *
 *===========================================================================*/

void NGramInit(NGram *self) {

    /* Initialize member variables. */
    self->usNumSets = 0;
    self->arrTokenSet = NULL;

    /* Assign the default member functions */
    self->generateModel = NGramGenerateModel;    

    return;
}


void NGramDeinit(NGram *self) {
    int      i, j;
    TokenSet *pTokenSet;

    if (self->arrTokenSet != NULL) {
        for (i = 0 ; i < self->usNumSets ; i++) {

            /* Free the TokenSet structure. */  
            if (self->arrTokenSet[i] != NULL) {
                pTokenSet = self->arrTokenSet[i];               

                /* Free the array of Token structures. */
                if (pTokenSet->arrToken != NULL) {
                    for (j = 0 ; j < pTokenSet->ulNumTokens ; j++) {
                        if (pTokenSet->arrToken[j] != NULL)
                            Free(pTokenSet->arrToken[j]);
                    }                        
                    Free(pTokenSet->arrToken);
                }
                Free(pTokenSet);
            }
        }
        Free(self->arrTokenSet);
    }

    return;
}


/**
 * NGramGenerateModel(): Generate the n-gram model based on the specified method.
 */
int NGramGenerateModel(NGram *self, const char *cszMethod, PEInfo *pPEInfo, RegionCollector *pRegionCollector) {

    return 0;
}


/*===========================================================================*
 *                Implementation for internal functions                      *
 *===========================================================================*/

int _FuncTokenFreqDescOrder(NGram *self, const char *cszMethod, PEInfo *pPEInfo, RegionCollector *pRegionCollector) {

    return 0;
}
