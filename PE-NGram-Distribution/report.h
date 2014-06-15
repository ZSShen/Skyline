#ifndef _REPORT_H_
#define _REPORT_H_

#include "util.h"
#include "except.h"
#include "pe_info.h"
#include "ngram.h"

typedef struct _Report {
    int (*createFolder)           (struct _Report*, const char*);
    int (*logEntropyDistribution) (struct _Report*, const char*);
    int (*logNGramModel)          (struct _Report*, const char*);
    int (*plotNGramModel)         (struct _Report*, const char*);
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
 * This function creates the folder to store all the related reports.
 *
 * @param   self        The pointer to the Report structure.
 * @param   cszPath     The path to the output folder.
 *
 * @return              0: The folder is created successfully.
 *                    < 0: Exception occurs while folder creation.
 */
int ReportCreateFolder(Report *self, const char *cszPath);


/**
 * This function logs the entropy distribution of all the sections.
 *
 * @param   self        The pointer to the Report structure.
 * @param   cszName     The name of the input sample.
 *
 * @return              0: The report is generated successfully.
 *                    < 0: Exception occurs while file creation or file writing.
 */
int ReportLogEntropyDistribution(Report *self, const char *cszName);


/**
 * This function logs the full n-gram model.
 * 
 * @param   self        The pointer to the Report structure.
 * @param   cszName     The name of the input sample.
 *
 * @return              0: The report is generated successfully.
 *                    < 0: Exception occurs while file creation or file writing.
 */
int ReportLogNGramModel(Report *self, const char *cszName);


/**
 * This function plots the visualized trend line of n-gram model with gnuplot utility.
 *
 * @param   self        The pointer to the Report structure.
 * @param   cszName     The name of the input sample.
 *
 * @return              0: The port is generated successfully.
 *                    < 0: Exception occurs while shell command execution.
 */
int ReportPlotNGramModel(Report *self, const char *cszName);

#endif
