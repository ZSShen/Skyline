#ifndef _POLICY_H_
#define _POLICY_H_

#include "util.h"

typedef struct RangePair_ {
    ulong               idxBgn, idxEnd;
    struct RangePair_   *next;
} RangePair;

typedef struct Region_ {
    int         idxSection;
    RangePair   *listRangePair;
} Region;

typedef struct PolicyRegion_ {
    int     iCountRegions, iNGramDimension;
    Region  *arrRegion;
} PolicyRegion;

// Policies to select block regions for n-gram model generation.
int PolicySelectBlockRegions(PEInfo*, const char*, PolicyRegion**);
int GetRegsFromMaxEntropySec(PEInfo*, PolicyRegion**);
int GetRegsFromMaxEntropySecWithinPlateaus(PEInfo*, PolicyRegion**);
int UninitPolicyRegion(PolicyRegion*);
int UninitListRangepair(RangePair*);

// Policies to generate n-gram model.
int PolicyGenerateModel(PEInfo*, const char*, PolicyRegion*);
int PutModelByDescFreqOrder(PEInfo*, PolicyRegion*);
int CollectTokensByPolicyRegions(PEInfo*, PolicyRegion*);
int MergeTokens(PEInfo*, NGramToken**);

// Sorting utilities.
int FuncCompareTokenDescOrder(const void*, const void*);

#endif