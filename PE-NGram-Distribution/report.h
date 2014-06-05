#ifndef _REPORT_H_
#define _REPORT_H_

#include "util.h"
#include "pe_info.h"

int ReportGenerate(PEInfo*, const char*);
int LogSectionEntropy(PEInfo*, const char*);
int LogNGramModel(PEInfo*, const char*);
int PlotNGramModel(PEInfo*, const char*);

/*
int ReportOutput(PEInfo*, char*);
int ReportLogSectionEntropy(PEInfo*, char*);
int ReportLogSectionNGramTokens(PEInfo*, char*);
int ReportLogNGramModel(PEInfo*, char*);
int ReportDrawNGramModel(PEInfo*, char*);

int ReportDumpSections(PEInfo*, int, int*, char*);
*/

#endif