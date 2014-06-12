#include "ngram.h"
/*===========================================================================*
 *                  Simulation for private variables                         *
 *===========================================================================*/
/* The maximum value of the n-gram token with the specified dimension. */
ulong _ulMaxValue;


/*===========================================================================*
 *                  Definition for internal functions                        *
 *===========================================================================*/

/**
 * This function generates the model by sorting the appearance frequencies of n-gram tokens
 * with descending order.
 *
 * @param   self                The pointer to the NGram structure.
 * @param   pPEInfo             The pointer to the to be analyzed PEInfo structure. 
 * @param   pRegionCollector    The pointer to the RegionCollector structure which stores all the selected features.
 *
 * @return                      0: The model is generated successfully.
 *                            < 0: Exception occurs while memory allocation.
 */
int _FuncTokenFreqDescOrder(NGram *self, PEInfo *pPEInfo, RegionCollector *pRegionCollector);


/*===========================================================================*
 *                Implementation for exported functions                      *
 *===========================================================================*/

void NGramInit(NGram *self) {

    /* Initialize member variables. */
    self->ulNumTokens = 0;
    self->arrToken = NULL;

    /* Assign the default member functions */
    self->setDimension = NGramSetDimension;
    self->generateModel = NGramGenerateModel;    

    return;
}


void NGramDeinit(NGram *self) {
    int     i;
    Token   *pToken;

    if (self->arrToken != NULL) {
        /* Free the array of Token structures. */
        for (i = 0 ; i < self->ulNumTokens ; i++) {
            pToken = self->arrToken[i];
            if (pToken != NULL)
                Free(pToken);
        }
        Free(self->arrToken);
    }

    return;
}


/**
 * NGramSetDimension(): Set the maximum value of the n-gram token with the specified dimension.
 */
void NGramSetDimension(NGram *self, uchar ucDimension) {

    _ulMaxValue = pow(UNI_GRAM_MAX_VALUE, ucDimension);
    return;
}


/**
 * NGramGenerateModel(): Generate the n-gram model based on the specified method.
 */
int NGramGenerateModel(NGram *self, const char *cszMethod, PEInfo *pPEInfo, RegionCollector *pRegionCollector) {

    if (cszMethod == NULL)
        _FuncTokenFreqDescOrder(self, pPEInfo, pRegionCollector);

    return 0;
}


/*===========================================================================*
 *                Implementation for internal functions                      *
 *===========================================================================*/

int _FuncTokenFreqDescOrder(NGram *self, PEInfo *pPEInfo, RegionCollector *pRegionCollector) {

    

    return 0;
}
