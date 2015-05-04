#ifndef _REPORT_H_
#define _REPORT_H_

#include "util.h"
#include "except.h"
#include "pe_info.h"
#include "ngram.h"

typedef struct _Report {
    int (*generateFolder)         (struct _Report*, const char*);
    int (*logEntropyDistribution) (struct _Report*, PEInfo*, const char*, const char*);
    int (*logNGramModel)          (struct _Report*, NGram*,  const char*, const char*);
    int (*plotNGramModel)         (struct _Report*, NGram*, const char*, const char*);
} Report;


/* Wrapper for Report structure initialization. */
#define Report_init(p)          try { \
                                    p = (Report*)Malloc(sizeof(Report)); \
                                    ReportInit(p); \
                                } catch (EXCEPT_MEM_ALLOC) { \
                                    p = NULL; \
                                } end_try;


/* Wrapper for Report structure deinitialization. */
#define Report_deinit(p)        if (p != NULL) { \
                                    ReportDeinit(p); \
                                    Free(p); \
                                    p = NULL; \
                                }


/* Constructor for Report structure. */
void ReportInit(Report *self);


/* Destructor for Report structure. */
void ReportDeinit(Report *self);


/**
 * This function generates the folder with the designated path to store reports.
 * Note that it will removes all the files in the folder if it already exists.
 *
 * @param   self            The pointer to the Report structure.
 * @param   cszDirPath      The path to the output folder.
 *
 * @return              0: The folder is generated successfully.
 *                    < 0: Exception occurs while traversing file system.
 */
int ReportGenerateFolder(Report *self, const char* cszDirPath);


/**
 * This function logs the entropy distribution of all the sections.
 *
 * @param   self            The pointer to the Report structure.
 * @param   pPEInfo         The pointer to the PEInfo structure.
 * @param   cszDirPath      The path to the output folder.
 * @param   cszSampleName   The name of the input sample.
 *
 * @return              0: The report is generated successfully.
 *                    < 0: Exception occurs while file creation or file writing.
 */
int ReportLogEntropyDistribution(Report *self, PEInfo *pPEInfo, const char *cszDirPath, const char *cszSampleName);


/**
 * This function logs the full n-gram model.
 * 
 * @param   self            The pointer to the Report structure.
 * @param   pNGram          The pointer to the NGram structure.
 * @param   cszDirPath      The path to the output folder.
 * @param   cszSampleName   The name of the input sample.
 *
 * @return              0: The report is generated successfully.
 *                    < 0: Exception occurs while file creation or file writing.
 */
int ReportLogNGramModel(Report *self, NGram *pNGram, const char *cszDirPath, const char *cszSampleName);


/**
 * This function plots the visualized trend line of n-gram model with gnuplot utility.
 *
 * @param   self            The pointer to the Report structure.
 * @param   pNGram          The pointer to the NGram structure.
 * @param   cszDirPath      The path to the output folder.
 * @param   cszSampleName   The name of the input sample.
 *
 * @return              0: The port is generated successfully.
 *                    < 0: Exception occurs while shell command execution.
 */
int ReportPlotNGramModel(Report *self, NGram *pNGram, const char *cszDirPath, const char *cszSampleName);

#endif
