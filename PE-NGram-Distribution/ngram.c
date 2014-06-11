#include "ngram.h"

/*===========================================================================*
 *                  Definition for internal functions                        *
 *===========================================================================*/



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

        /* Free all the TokenSet structures. */              
        for (i = 0 ; i < self->usNumSets ; i++) {
            if (self->arrTokenSet[i] != NULL) {
                pTokenSet = self->arrTokenSet[i];               

                /* Free all the Token records. */
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
