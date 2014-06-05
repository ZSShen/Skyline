#ifndef _PE_INFO_H_
#define _PE_INFO_H_

#include "util.h"

// Data structures to store PE related information.
typedef struct PEHeader_ {
    ulong   ulHeaderOffset;
    int     iCountSections;
} PEHeader;

typedef struct EntropyInfo_ {
    ulong   ulSizeArray;
    double  *arrEntropy;
    double  dMaxEntropy, dAvgEntropy, dMinEntropy;
} EntropyInfo;

typedef struct SectionInfo_ {
    ulong       ulRawSize, ulRawOffset, ulCharacteristics;
    ulong       ulSizeTokenArray;
    char        szNormalizedName[SECTION_HEADER_SECTION_NAME_SIZE + 1]; 
    char        szRawName[SECTION_HEADER_SECTION_NAME_SIZE + 1];
    EntropyInfo *pEntropyInfo;    
} SectionInfo;


// Data structures to store n-gram analysis result.
typedef struct NGramToken_ {
    ulong ulValue, ulFrequency;
} NGramToken;

typedef struct NGramTokenSet_ {
    int         idxSection;
    NGramToken  *arrToken;
} NGramTokenSet;

typedef struct NGramSingleSlice_ {
    NGramToken  tokenDenominator, tokenNumerator;
    double      dFactor;
} NGramSingleSlice;

typedef struct NGramModel_ {
    int                 iCountSets;
    ulong               ulNGramSpace, ulModelRealSize;
    NGramTokenSet       *arrTokenSet;
    NGramSingleSlice    *arrSlice;
} NGramModel;


// Summarized data structure.
typedef struct PEInfo_ {
    char            *szSample;
    FILE            *fpFile;
    PEHeader        *pPEHeader;
    SectionInfo     **arrSectionInfo;
    NGramModel      *pNGramModel;
} PEInfo;


int PEInfoInit(PEInfo**, const char*);
int PEInfoUninit(PEInfo*);

int PEInfoParseHeaders(PEInfo*);
int PEInfoCalculateSectionEntropy(PEInfo*);
int PEInfoCollectNGramTokens(PEInfo*, int);

int FuncCompareNGramToken(const void*, const void*);

#endif