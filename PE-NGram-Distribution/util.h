#ifndef _UTIL_H_
#define _UTIL_H_

#if defined(_WIN32)
    #include <windows.h>
    #include <shellapi.h>
#elif defined(__linux__)
    #include <errno.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <setjmp.h>
#include <assert.h>
#include <time.h>


typedef unsigned char   uchar;
typedef unsigned int    uint;
typedef unsigned long   ulong;

/*===========================================================================*
 *                    Wrapper for utility functions                          *
 *===========================================================================*/
#define Log                         WriteLog(__FILE__, __LINE__, __FUNCTION__,
#define Log0(p0)                    Log p0)
#define Log1(p0, p1)                Log p0, p1)
#define Log2(p0, p1, p2)            Log p0, p1, p2)
#define Log3(p0, p1, p2, p3)        Log p0, p1, p2, p3)

#define Malloc(p0)                  MemAlloc (p0,     __FILE__, __LINE__, __FUNCTION__)
#define Calloc(p0, p1)              MemCalloc(p0, p1, __FILE__, __LINE__, __FUNCTION__)
#define Free  (p0)                  MemFree  (p0)

#define Fopen (p0, p1)              FileOpen (p0, p1,         __FILE__, __LINE__, __FUNCTION__)
#define Fread (p0, p1, p2, p3)      FileRead (p0, p1, p2, p3, __FILE__, __LINE__, __FUNCTION__)
#define Fwrite(p0, p1, p2, p3)      FileWrite(p0, p1, p2, p3, __FILE__, __LINE__, __FUNCTION__)
#define Fseek (p0, p1, p2)          FileSeek (p0, p1, p2,     __FILE__, __LINE__, __FUNCTION__)
#define Fclose(p0)                  FileClose(p0)


/*===========================================================================*
 *                       Definition of constants                             *
 *===========================================================================*/
/* Buffer size. */
#define BUF_SIZE_LARGE              (4096)
#define BUF_SIZE_MID                (512)
#define BUF_SIZE_SMALL              (128)
#define BUF_SIZE_TINY               (8)

/* Maximum number of lines in a single write request. */
#define BATCH_WRITE_LINE_COUNT      (80)

/* Number of bytes for each data unit. */
#define DATATYPE_SIZE_DWORD         (4)
#define DATATYPE_SIZE_WORD          (2)

/* Byte unit related information. */
#define SHIFT_RANGE_8BIT            (8)     /* The number of shifted bits in one byte. */
#define BIT_MOST_SIGNIFICANT        (7)     /* The position of most significant bit in one byte. */

/* DOS(MZ) header related information. */
#define DOS_HEADER_SIZE                     (0x40)  /* The size of DOS (MZ) header. */
#define DOS_HEADER_OFF_PE_HEADER_OFFSET     (0x3c)  /* The starting offset of PE header. */

/* PE header related information. */
#define PE_HEADER_SIZE                      (0x18)  /* The size of PE header. */
#define PE_HEADER_OFF_NUMBER_OF_SECTIONS    (0x6)   /* The number of sections. */
#define PE_HEADER_OFF_SIZE_OF_OPT_HEADER    (0x14)  /* The size of PE optional header. */

/* Section header related information. */
#define SECTION_HEADER_PER_ENTRY_SIZE       (0x28)  /* The size of each section entry. */
#define SECTION_HEADER_OFF_RAW_SIZE         (0x10)  /* The raw size of the section. */
#define SECTION_HEADER_OFF_RAW_OFFSET       (0x14)  /* The raw offset of the section. */
#define SECTION_HEADER_OFF_CHARS            (0x24)  /* The characteristics of the section*/
#define SECTION_HEADER_SECTION_NAME_SIZE    (0x8)   /* The maximum length of the section name. */

/* Criterions for section entroy calculation. */
#define ENTROPY_BLK_SIZE                    (256)   /* The required number of bytes for entropy calculation. */
#define ENTROPY_LOG_BASE                    (2)     /* The basis of logarithm for entropy calculation. */

/* Criterions for n-gram calculation. */
#define UNI_GRAM_MAX_VALUE                  (256)   /* The maximum value of n-gram with dimension one. */

/* The names of each kinds of reports. */
#define REPORT_NAME_TXT_SECTION_ENTROPY     "entropy.txt"
#define REPORT_NAME_TXT_NGRAM_MODEL         "ngram_model.txt"
#define REPORT_NAME_PNG_NGRAM_MODEL         "ngram_model.png"
#define REPORT_NAME_GNU_PLOT_SCRIPT         "plot_script"

/* Criterions for image-based report. */
#define REPORT_IMAGE_SIZE_WIDTH             (1920)
#define REPORT_IMAGE_SIZE_HEIGHT            (1080)

/* Path separator. */
#if defined(_WIN32)
    #define OS_PATH_SEPARATOR               '\\'
#elif defined(__linux__)
    #define OS_PATH_SEPARATOR               '/'
#endif

/* Strategies for entropy block selection. */
#define SELECT_PLATEAUS_WITHIN_MAX_ENTROPY_SEC "PLAT_IN_MAX_SEC"


/**
 * This function print the designated log message.
 * 
 * @param   cszPathCode     The path of the source code. 
 * @param   iLineNo         The line number of the statement. 
 * @param   cszFunc         The name of the function.
 * @param   cszFormat       The string format of the log message.
 */
void WriteLog(const char *cszPathCode, int iLineNo, const char *cszFunc, const char *cszFormat, ...);


/* Wrapper for memory manipulation utilities. */
void* MemAlloc(size_t, const char*, const int, const char*);
void* MemCalloc(size_t, size_t, const char*, const int, const char*);
void MemFree(void*);

/* Wrapper for file manipulation utilties. */
FILE* FileOpen(const char*, const char*, const char*, const int, const char*);
size_t FileRead(void*, size_t, size_t, FILE*, const char*, const int, const char*);
size_t FileWrite(void*, size_t, size_t, FILE*, const char*, const int, const char*);
int FileSeek(FILE*, long, int, const char*, const int, const char*);
int FileClose(FILE*);

#endif
