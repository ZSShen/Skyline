#include "ngram.h"


/*---------------------------------------------------------------------------*
 *                          Plugin Objective                                 *
 *                                                                           *
 *  This plugin generates the n-gram model by sorting the appearance         *
 *  frequencies of n-gram tokens with descending order.                      *
 *---------------------------------------------------------------------------*/


/*===========================================================================*
 *                Implementation for exported functions                      *
 *===========================================================================*/
/**
 * This function is the entry point of the plugin.
 *
 * @param   pNGram              The pointer to the NGram structure.
 *                              The plugin can reference the n-gram tokens from
 *                              this structure and then put the model into it.
 * @param   ulMaxValue          The maximum n-gram value. 
 * 
 * @return                      0: The model is generated successfully.
 *                            < 0: Exception occurs while memory allocation.
 */
int run(NGram *pNGram, ulong ulMaxValue) {
    int     rc, i, j;
    double  dScore;
    Token   *pToken, *pDenominator, *pNumerator;

    rc = 0;
    try {
        /* Sort the tokens. */
        qsort(pNGram->arrToken, ulMaxValue, sizeof(Token*), _CompTokenFreqDescOrder);

        /* Adjust the size of arrToken to eliminate NULL elements. */
        pNGram->arrToken = Realloc(pNGram->arrToken, sizeof(Token*) * pNGram->ulNumTokens);

        /* Ignore the dummy tokens: (ff)+ and (00)+. */
        pNGram->ulNumSlices = pNGram->ulNumTokens - 2;
        pNGram->arrSlice = NULL;
        pNGram->arrSlice = (Slice**)Calloc(pNGram->ulNumSlices, sizeof(Slice*));
        for (i = 0 ; i < pNGram->ulNumSlices ; i++)
            pNGram->arrSlice[i] = NULL; 
    
        /* Choose the most frequently appearing token as the denominator. */
        for (i = 0 ; i < pNGram->ulNumTokens ; i++) {
            pToken = pNGram->arrToken[i];
            if ((pToken->ulValue != 0) && (pToken->ulValue != (ulMaxValue - 1))) {
                pDenominator  = pToken;
                break;
            }
        }

        /* Collect the slices. */
        for (i = 0, j = 0; i < pNGram->ulNumTokens ; i++) {
            pToken = pNGram->arrToken[i];
            if ((pToken->ulValue != 0) && (pToken->ulValue != (ulMaxValue - 1))) {
                pNumerator = pToken;
                
                pNGram->arrSlice[j] = (Slice*)Malloc(sizeof(Slice));
                pNGram->arrSlice[j]->pDenominator = pDenominator;
                pNGram->arrSlice[j]->pNumerator = pNumerator;

                dScore = (double)pNumerator->ulFrequency / (double)pDenominator->ulFrequency;
                pNGram->arrSlice[j]->dScore = dScore;                

                j++;
            }
        }
    } catch(EXCEPT_MEM_ALLOC) {
        rc = -1;
    } end_try;
    
    return rc;
}


/*===========================================================================*
 *                Implementation for internal functions                      *
 *===========================================================================*/
/**
 * This function hints the qsort() library to sort the n-gram tokens by their appearance frequency
 * in descending order.
 *
 * @param   ppSrc        The pointer to the pointer indexing to the source token.
 * @param   ppDst        The pointer to the pointer indexing to the target token.
 *
 * @return             < 0: The source token must go before the target one.
 *                       0: The source and target tokens do not need to change their order.
 *                     > 0: The source token must go after the target one. 
 */
int _CompTokenFreqDescOrder(const void *ppSrc, const void *ppTge) {
    Token *pSrc, *pTge;

    pSrc = *(Token**)ppSrc;
    pTge = *(Token**)ppTge;

    if (pSrc == NULL) {
        if (pTge == NULL)
            return 0;
        else
            return 1;
    } else {
        if (pTge == NULL)
            return -1;
        else {
            if (pSrc->ulFrequency == pTge->ulFrequency)
                return 0;
            else {
                if (pSrc->ulFrequency < pTge->ulFrequency)
                    return 1;
                else
                    return -1;
            }
        }
    }
}

