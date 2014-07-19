#ifndef _PE_INFO_H_
#define _PE_INFO_H_

#include "util.h"
#include "except.h"

/* Structure to store the PE header information. */
typedef struct _PEHeader {
    ulong   ulHeaderOffset;
    ushort  usNumSections;
} PEHeader;


/* Structure to store the entroy data of a section. */
typedef struct _EntropyInfo {
    ulong   ulNumBlks;
    double  *arrEntropy;
    double  dMaxEntropy, dAvgEntropy, dMinEntropy;
} EntropyInfo;


/* Structure to store the section information. */
typedef struct _SectionInfo {
    ulong       ulRawSize, ulRawOffset, ulCharacteristics;
    uchar       uszNormalizedName[SECTION_HEADER_SECTION_NAME_SIZE + 1]; 
    uchar       uszOriginalName[SECTION_HEADER_SECTION_NAME_SIZE + 1];
    EntropyInfo *pEntropyInfo;    
} SectionInfo;


/* Structure to store the complete analysis result for a PE file. */
typedef struct _PEInfo {
    char          *szSampleName;
    FILE          *fpSample;
    PEHeader      *pPEHeader;
    SectionInfo   **arrSectionInfo;

    int     (*openSample)              (struct _PEInfo*, const char*);
    int     (*parseHeaders)            (struct _PEInfo*);
    int     (*calculateSectionEntropy) (struct _PEInfo*);
    void    (*dump)                    (struct _PEInfo*);
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


/**
 * This function collects the information from MZ header, PE header, PE option header,
 * and all the section headers.
 *
 * @param   self            The pointer to the PEInfo structure.
 *
 * @return                  0: The information is collected successfully.
 *                        < 0: Exception occurs while file accessing or memory allocation.
 */
int PEInfoParseHeaders(PEInfo *self);


/**
 * This function calculates and collects the entropy data of each section.
 *
 * @param   self            The pointer to the PEInfo structure.
 *
 * @param                   0: The entropy data is collected successfully.
 *                        < 0: Exception occurs while file accessing or memory allocation.
 */
int PEInfoCalculateSectionEntropy(PEInfo *self);


/**
 * This function dumps the information recorded from the input sample for debug.
 *
 * @param   self            The pointer to the PEInfo structure.
 */
void PEInfoDump(PEInfo *self);

#endif
