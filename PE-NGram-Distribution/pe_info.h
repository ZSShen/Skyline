#ifndef _PE_INFO_H_
#define _PE_INFO_H_

#include "util.h"


/* Structure to store the PE header information. */
typedef struct _PEHeader {
    ulong   ulHeaderOffset;
    int     iNumSections;
} PEHeader;


/* Structure to store the entroy data of a section. */
typedef struct _EntropyInfo {
    ulong   ulSizeArray;
    double  *arrEntropy;
    double  dMaxEntropy, dAvgEntropy, dMinEntropy;
} EntropyInfo;


/* Structure to store the section information. */
typedef struct _SectionInfo {
    ulong       ulRawSize, ulRawOffset, ulCharacteristics;
    char        szNormalizedName[SECTION_HEADER_SECTION_NAME_SIZE + 1]; 
    char        szOriginalName[SECTION_HEADER_SECTION_NAME_SIZE + 1];
    EntropyInfo *pEntropyInfo;    
} SectionInfo;


/* Structure to store the n-gram token. */
typedef struct _NGramToken {
    ulong ulValue, ulFrequency;
} NGramToken;


/* Structure to store the complete analysis result for a PE file. */
typedef struct _PEInfo {
    char          *szSampleName;
    FILE          *fpSample;
    PEHeader      *pPEHeader;
    SectionInfo   **arrSectionInfo;

    int     (*openSample) (struct _PEInfo*, const char*);
} PEInfo;


/* Wrapper for PEInfo initialization. */
#define PEInfo_init(p)          try { \
                                    p = (PEInfo*)Malloc(sizeof(PEInfo)); \
                                    PEInfoInit(p); \
                                } catch (EXCEPT_MEM_ALLOC) { \
                                    p = NULL; \
                                } end_try; 


/* Wrapper for PEInfo deinitialization. */
#define PEInfo_deinit(p)        if (p != NULL) { \
                                    PEInfoDeinit(p); \
                                    Free(p); \
                                    p = NULL; \
                                }


/* Constructor for PEInfo structure. */
void PEInfoInit(PEInfo *pPEInfo);


/* Destructure for PEInfo structure. */
void PEInfoDeinit(PEInfo *pPEInfo);


/**
 * This function opens the specified sample for analysis.
 *
 * @param   self            The pointer to the PEInfo structure.
 * @param   cszSamplePath   The path of the specified sample.
 * 
 * @return                  0: The sample is successfully opened.
 *                        < 0: The sample cannot be opend.
 */
int PEInfoOpenSample(PEInfo *self, const char *cszSamplePath);


/*
int PEInfoParseHeaders(PEInfo*);
int PEInfoCalculateSectionEntropy(PEInfo*);
int PEInfoCollectNGramTokens(PEInfo*, int);
int FuncCompareNGramToken(const void*, const void*);
*/


#endif
