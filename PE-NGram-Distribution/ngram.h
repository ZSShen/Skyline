#ifndef _NGRAM_H_
#define _NGRAM_H_

#include "util.h"
#include "except.h"
#include "pe_info.h"
#include "region.h"

/* Structure to record the value and the appearance frequency of a specific n-gram token. */
typedef struct _Token {
    ulong ulValue, ulFrequency;
} Token;


/* Structure to store all the ngram tokens of a specific section. */
typedef struct _TokenSet {
    ushort  usIdxSection;
    ulong   ulNumTokens;
    Token   **arrToken;
} TokenSet;


/* Structure to store all the information of n-gram model for the input sample. */
typedef struct _NGram {
    ushort      usNumSets;
    ulong       ulValueSpace;
    TokenSet    **arrTokenSet;

    int (*generateModel) (struct _NGram*, const char*, PEInfo*, RegionCollector*);
} NGram;


/* Constructor for the NGram structure. */
void NGramInit(NGram *self);


/* Destructor for the NGram structure. */
void NGramDeinit(NGram *self);


/**
 * This function generates the n-gram model based on the specified method.
 *
 * @param   self                The pointer to the NGram structure.
 * @param   cszMethod           The string describing the model generation method.
 * @param   pPEInfo             The pointer to the to be analyzed PEInfo structure. 
 * @param   pRegionCollector    The pointer to the RegionCollector structure which stores all the selected features.
 *
 * @return                      0: The model is generated successfully.
 *                            < 0: Exception occurs while memory allocation.
 */
int NGramGenerateModel(NGram *self, const char *cszMethod, PEInfo *pPEInfo, RegionCollector *pRegionCollector);

/*===========================================================================*
 *                 Supported Model Generation Methods                        *
 *===========================================================================*/
/*
 *
 *
 *
 *
 *
 */ 

#endif
