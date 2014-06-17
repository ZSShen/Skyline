#include "report.h"


/* Constructor for Report structure. */
void ReportInit(Report *self) {

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
 * ReportLogEntropyDistribution(): Log the entropy distribution of all the sections.
 */
int ReportLogEntropyDistribution(Report *self, const char *cszPath, const char *cszName) {
    int rc;

    rc = 0;

    try {
        #if defined(_WIN32)

        #elif defined(__linux__)
            Mkdir(cszPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        #endif
    
    } catch(EXCEPT_IO_DIR_MAKE) {
        rc = -1;
    } end_try;

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


