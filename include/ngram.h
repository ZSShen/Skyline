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


/* Structure to store the single record of n-gram model. */
typedef struct _Slice {
    Token  *pDenominator, *pNumerator;
    double dScore;
} Slice;


/* Structure to store all the information of n-gram model for the input sample. */
typedef struct _NGram {
    ulong   ulNumTokens, ulNumSlices;
    Token   **arrToken;
    Slice   **arrSlice;

    void *hdlePlug;
    int (*entryPlug) (struct _NGram*, ulong);

    int  (*loadPlugin)    (struct _NGram*, const char*);
    int  (*unloadPlugin)  (struct _NGram*);
    void (*setDimension)  (struct _NGram*, uchar ucDimension);
    int  (*generateModel) (struct _NGram*, PEInfo*, RegionCollector*);
    void (*dump)          (struct _NGram*);
} NGram;


/* Wrapper for NGram initialization. */
#define NGram_init(p)       try {                                               \
                                p = (NGram*)Malloc(sizeof(NGram));              \
                                NGramInit(p);                                   \
                            } catch (EXCEPT_MEM_ALLOC) {                        \
                                p = NULL;                                       \
                            } end_try;


/* Wrapper for NGram deinitialization. */
#define NGram_deinit(p)     if (p != NULL) {                                    \
                                NGramDeinit(p);                                 \
                                Free(p);                                        \
                                p = NULL;                                       \
                            }


/* Constructor for NGram structure. */
void NGramInit(NGram *self);


/* Destructor for NGram structure. */
void NGramDeinit(NGram *self);


/**
 * This function loads the model generation plugin.
 *
 * @param   self            The pointer to the NGram structure.
 * @param   cszName         The plugin name.
 *
 * @return                  0: The plugin is loaded successfully.
 *                        < 0: The plugin cannot be loaded.
 */
int NGramLoadPlugin(NGram *self, const char *cszName);


/**
 * This function unloads the model generation plugin.
 *
 * @param   self            The pointer to the NGram structure.
 * @return                  0: The plugin is unloaded successfully.
 *                        < 0: The plugin cannot be unloaded.
 */
int NGramUnloadPlugin(NGram *self);


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
 * @param   pPEInfo             The pointer to the to be analyzed PEInfo structure.
 * @param   pRegionCollector    The pointer to the RegionCollector structure which stores all the selected features.
 *
 * @return                      0: The model is generated successfully.
 *                            < 0: Exception occurs while memory allocation.
 */
int NGramGenerateModel(NGram *self, PEInfo *pPEInfo, RegionCollector *pRegionCollector);


/**
 * This function dumps the information recorded from the generated n-gram tokens.
 *
 * @param   self            The pointer to the NGram structure.
 */
void NGramDump(NGram *self);

#endif
