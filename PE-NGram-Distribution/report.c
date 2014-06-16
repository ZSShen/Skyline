#include "report.h"


/* Constructor for Report structure. */
void ReportInit(Report *self) {

    self->createFolder = ReportCreateFolder;
    self->logEntropyDistribution = ReportLogEntropyDistribution;
    self->logNGramModel = ReportLogNGramModel;
    self->plotNGramModel = ReportPlotNGramModel;

    return;
}


/* Destructor for Report structure. */
void ReportDeinit(Report *self) {

    return;
}


/**
 * ReportCreateFolder(): Create the folder to store all the related reports.
 */
int ReportCreateFolder(Report *self, const char *cszPath) {
    int rc;

    rc = 0;

    return rc;
}


/**
 * ReportLogEntropyDistribution(): Log the entropy distribution of all the sections.
 */
int ReportLogEntropyDistribution(Report *self, const char *cszName) {
    int rc;

    rc = 0;

    return rc;
}


/**
 * ReportLogNGramModel(): Log the full n-gram model.
 */
int ReportLogNGramModel(Report *self, const char *cszName) {
    int rc;

    rc = 0;

    return rc;
}


/**
 * ReportPlotNGramModel(): Plot the visualized trend line of n-gram model with gnuplot utility.
 */
int ReportPlotNGramModel(Report *self, const char *cszName) {
    int rc;

    rc = 0;

    return rc;
}


