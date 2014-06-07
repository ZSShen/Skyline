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
typedef struct PEInfo_ {
    char          *szSamplePath;
    FILE          *fpSample;
    PEHeader      *pPEHeader;
    SectionInfo   **arrSectionInfo;
} PEInfo;


/* Wrapper for PEInfo initialization. */
#define PEInfo_init(p)          try { \
                                    p = (PEInfo*)Malloc(sizeof(PEInfo)); \
                                } catch (EXCEPT_MEM_ALLOC) { \
                                    p = NULL;                \
                                } end_try; 


/* Constructor for PEInfo structure. */
void PEInfoInit(PEInfo *pPEInfo);


/* Destructure for PEInfo structure. */
void PEInfoDeinit(PEInfo *pPEInfo);


/*
int PEInfoParseHeaders(PEInfo*);
int PEInfoCalculateSectionEntropy(PEInfo*);
int PEInfoCollectNGramTokens(PEInfo*, int);
int FuncCompareNGramToken(const void*, const void*);
*/


#endif
