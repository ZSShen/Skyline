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


/* Structure to store all the information of n-gram model for the input sample. */
typedef struct _NGram {
    ulong   ulNumTokens;
    Token   **arrToken;

    void (*setDimension)  (struct _NGram*, uchar ucDimension);
    int  (*generateModel) (struct _NGram*, const char*, PEInfo*, RegionCollector*);
} NGram;


/* Wrapper for NGram initialization. */
#define NGram_init(p)       try { \
                                p = (NGram*)Malloc(sizeof(NGram)); \
                                NGramInit(p); \
                            } catch (EXCEPT_MEM_ALLOC) { \
                                p = NULL; \
                            } end_try;


/* Wrapper for NGram deinitialization. */
#define NGram_deinit(p)     if (p != NULL) { \
                                NGramDeinit(p); \
                                Free(p); \
                                p = NULL; \
                            }


/* Constructor for the NGram structure. */
void NGramInit(NGram *self);


/* Destructor for the NGram structure. */
void NGramDeinit(NGram *self);


/**
 * This function sets the maximum value of the n-gram token with the specified dimension.
 *
 * @param   self            The pointer to the NGram structure.
 * @param   ucDimension     The user-specified dimension.
 */
void NGramSetDimension(NGram *self, uchar ucDimension);


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
