#include "ngram.h"


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
 * @param   pNGram              The pointer to the NGram structure.
 *                              The plugin can reference the n-gram tokens from
 *                              this structure and then put the model into it.
 * @param   ulMaxValue          The maximum n-gram value. 
 * 
 * @return                      0: The model is generated successfully.
 *                            < 0: Exception occurs while memory allocation.
 */
int run(NGram *pNGram, ulong ulMaxValue) {
    int rc;

    rc = 0;
    
    return rc;
}
