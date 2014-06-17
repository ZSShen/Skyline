#include "util.h"
#include "except.h"


/**
 * WriteLog(): Print the designated log message.
 */
void WriteLog(const char* cszPathFile, int iLineNo, const char* cszFunc, const char* cszFormat, ...) {
    char      szLogBuf[BUF_SIZE_MID];
    int       iLen;
    time_t    nTime;        
    va_list   varArgument;
    char      *szTime;    
    struct tm *tmTime;

    memset(szLogBuf, 0, sizeof(char) * BUF_SIZE_MID);
    va_start(varArgument, cszFormat);
    iLen = vsnprintf(szLogBuf, sizeof(szLogBuf), cszFormat, varArgument);
    va_end(varArgument);

    if((iLen == -1) || (iLen >= (int)sizeof(szLogBuf))) {
	    iLen = sizeof(szLogBuf) - 1;
	    szLogBuf[iLen] = 0;
    } else if(iLen == 0) {
	    iLen = 0;
	    szLogBuf[0] = 0;
    }

    time(&nTime);
    tmTime = localtime(&nTime);
    szTime = asctime(tmTime);

    printf("[%s, %d, %s] %s%s", cszPathFile, iLineNo, cszFunc, szTime, szLogBuf);

    return;
}


/**
 * MemAlloc(): Wrapper function for malloc().
 */
void* MemAlloc(size_t nLength, const char *cszPathFile, const int iLineNo, const char* cszFunc) {
    void *ptr;

    assert(nLength > 0);
	ptr = malloc(nLength);
    if (ptr == NULL)
        throw(EXCEPT_MEM_ALLOC);

    return ptr;
}


/**
 * MemCalloc(): Wrapper function for calloc().
 */
void* MemCalloc(size_t nLength, size_t nSize, const char *cszPathFile, const int iLineNo, const char* cszFunc) {
    void *ptr;
    
    assert(nLength > 0);
    ptr = calloc(nLength, nSize);
    if (ptr == NULL)
        throw(EXCEPT_MEM_ALLOC);

    return ptr;
}


/**
 * MemRealloc(): Wrapper function for realloc().
 */
void* MemRealloc(void *pOld, size_t nLength, const char *cszPathFile, const int iLineNo, const char* cszFunc) {
    void *pNew;

    assert(nLength > 0);
    pNew = realloc(pOld, nLength);
    if (pNew == NULL)
        throw(EXCEPT_MEM_ALLOC);

    return pNew;
}


/**
 * MemFree(): Wrapper function for free().
 */
void MemFree(void *ptr) {

    if (ptr != NULL)
        free(ptr);
}


/**
 * MemCopy(): Copy the contents of the source memory space to the target one.
 */
void MemCopy(void *pTge, const void *pSrc, size_t nLength, size_t nSize) {

    memset(pTge, 0, (nLength + 1) * nSize);
    memcpy(pTge, pSrc, nLength * nSize);
    return;
}



/**
 * FileOpen(): Wrapper function for fopen().
 */
FILE* FileOpen(const char *cszPath, const char *cszMode, const char *cszPathFile, const int iLineNo, const char* cszFunc) {
    FILE *fptr;
    
    fptr = fopen(cszPath, cszMode);
    if (fptr == NULL)
        throw(EXCEPT_IO_FILE_OPEN);

    return fptr;
}


/**
 * FileRead(): Wrapper function for fread().
 */
size_t FileRead(void *ptr, size_t nSize, size_t nLength, FILE *fptr, const char *cszPathFile, const int iLineNo, const char *cszFunc) {
    size_t  nRead;
    int     rc;
    
    nRead = fread(ptr, nSize, nLength, fptr);
    if (nRead != (nSize * nLength)) {
        rc = ferror(fptr);
        if (rc != 0)
            throw(EXCEPT_IO_FILE_READ);
    }
    
    return nRead;
}


/**
 * FileWrite(): Wrapper function for fwrite().
 */
size_t FileWrite(void *ptr, size_t nSize, size_t nLength, FILE *fptr, const char *cszPathFile, const int iLineNo, const char *cszFunc) {
    size_t  nWrite;
    int     rc;
    
    nWrite = fwrite(ptr, nSize, nLength, fptr);
    if (nWrite != (nSize * nLength)) {
        rc = ferror(fptr);
        if (rc != 0)
            throw(EXCEPT_IO_FILE_WRITE);
    }

    return nWrite;
}


/**
 * FileSeek(): Wrapper function for fseek().
 */
int FileSeek(FILE *fptr, long iOffset, int iOrigin, const char *cszPathFile, const int iLineNo, const char *cszFunc) {
    int rc;
    
    rc = fseek(fptr, iOffset, iOrigin);
    if(rc != 0)
        throw(EXCEPT_IO_FILE_SEEK);
    
    return rc;
}


/**
 * FileClose(): Wrapper function for fclose().
 */
int FileClose(FILE *fptr) {
    int rc; 

   if (fptr != NULL)
        rc = fclose(fptr);
    
    return rc;
}


/**
 * DirMake(): Wrapper function for mkdir().
 */
int DirMake(const char *cszPathDir, mode_t mode, const char *cszPathFile, const int iLineNo, const char *cszFunc) {
    int rc;

    rc = mkdir(cszPathDir, mode);
    if (rc != 0) {
        if (errno != EEXIST)
            throw(EXCEPT_IO_DIR_MAKE);
    }

    return rc;
}

