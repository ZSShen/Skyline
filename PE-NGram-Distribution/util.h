#ifndef _UTIL_H_
#define _UTIL_H_

#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <setjmp.h>
#include <assert.h>

typedef unsigned char   uchar;
typedef unsigned int    uint;
typedef unsigned long   ulong;

#define Log							WriteLog(__FILE__, __LINE__, __FUNCTION__,
#define Log0(p0)					Log p0)
#define Log1(p0, p1)				Log p0, p1)
#define Log2(p0, p1, p2)			Log p0, p1, p2)
#define Log3(p0, p1, p2, p3)		Log p0, p1, p2, p3)

#define Malloc(p0)                  MemAlloc(p0, __FILE__, __LINE__, __FUNCTION__)
#define Calloc(p0, p1)              MemCalloc(p0, p1, __FILE__, __LINE__, __FUNCTION__)
#define Free(p0)                    MemFree(p0)

#define Fopen(p0, p1)               FileOpen(p0, p1, __FILE__, __LINE__, __FUNCTION__)
#define Fread(p0, p1, p2, p3)       FileRead(p0, p1, p2, p3, __FILE__, __LINE__, __FUNCTION__)
#define Fwrite(p0, p1, p2, p3)      FileWrite(p0, p1, p2, p3, __FILE__, __LINE__, __FUNCTION__)
#define Fseek(p0, p1, p2)           FileSeek(p0, p1, p2, __FILE__, __LINE__, __FUNCTION__)
#define Fclose(p0)                  FileClose(p0)


#define BUF_SIZE_LARGE              (4096)
#define BUF_SIZE_MID                (512)
#define BUF_SIZE_SMALL              (128)
#define BUF_SIZE_TINY               (8)

#define DATATYPE_SIZE_DWORD         (4)
#define DATATYPE_SIZE_WORD          (2)

#define SHIFT_RANGE_8BIT            (8)

// The size of DOS header.
// The relative offset of each DOS header member.
#define DOS_HEADER_SIZE                     (0x40)
#define DOS_HEADER_OFF_PE_HEADER_OFFSET     (0x3c)

// The size of PE header.
// The relative offset of each PE header member.
#define PE_HEADER_SIZE                      (0x18)
#define PE_HEADER_OFF_NUMBER_OF_SECTIONS    (0x6)
#define PE_HEADER_OFF_SIZE_OF_OPT_HEADER    (0x14)


// The size of each entry of section header.
// The relative offset of each member of a single header entry.
#define SECTION_HEADER_PER_ENTRY_SIZE       (0x28)
#define SECTION_HEADER_OFF_RAW_SIZE         (0x10)
#define SECTION_HEADER_OFF_RAW_OFFSET       (0x14)
#define SECTION_HEADER_OFF_CHARS            (0x24)
#define SECTION_HEADER_SECTION_NAME_SIZE    (8)

#define ENTROPY_BLK_SIZE                    (256)
#define ENTROPY_LOG_BASE                    (2)

#define UNI_GRAM_MAX_VALUE                  (256)

#define BIT_MOST_SIGNIFICANT                (7)

// The names of output reports.
#define REPORT_NAME_TXT_SECTION_ENTROPY     "entropy.txt"
#define REPORT_NAME_TXT_NGRAM_MODEL         "ngram_model.txt"
#define REPORT_NAME_PNG_NGRAM_MODEL         "ngram_model.png"
#define REPORT_NAME_GNU_PLOT_SCRIPT         "plot_script"

#define REPORT_IMAGE_SIZE_WIDTH             (1920)
#define REPORT_IMAGE_SIZE_HEIGHT            (1080)

#define BATCH_WRITE_LINE_COUNT              (80)


// For path separator.
#ifdef _WIN32
    #define OS_PATH_SEPARATOR               '\\'
#else
    #define OS_PATH_SEPARATOR               '/'
#endif


// For policy of entropy block selection.
#define SELECT_PLATEAUS_WITHIN_MAX_ENTROPY_SEC "PLAT_IN_MAX_SEC"


int GenerateErrorMessage(void*);
int WriteLog(const char*, int, const char*, const char*, ...);


// Wrapped memory manipulations.
void* MemAlloc(size_t, const char*, const int, const char*);
void* MemCalloc(size_t, size_t, const char*, const int, const char*);
void  MemFree(void*);

// Wrapped file manipulations.
FILE* FileOpen(const char*, const char*, const char*, const int, const char*);
size_t FileRead(void*, size_t, size_t, FILE*, const char*, const int, const char*);
size_t FileWrite(void*, size_t, size_t, FILE*, const char*, const int, const char*);
int FileSeek(FILE*, long, int, const char*, const int, const char*);
int FileClose(FILE*);

#endif
