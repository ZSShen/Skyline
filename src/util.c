#include "util.h"
#include "except.h"


void WriteLog(const char* cszPathSrc, int iLineNo, const char* cszFunc, const char* cszFormat, ...) {
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

    printf("[%s, %d, %s] %s%s", cszPathSrc, iLineNo, cszFunc, szTime, szLogBuf);

    return;
}

void* MemAlloc(size_t nLength, const char *cszPathSrc, const int iLineNo, const char* cszFunc) {
    void *ptr;

    assert(nLength > 0);
	ptr = malloc(nLength);
    if (ptr == NULL)
        throw(EXCEPT_MEM_ALLOC);

    return ptr;
}

void* MemCalloc(size_t nLength, size_t nSize, const char *cszPathSrc, const int iLineNo, const char* cszFunc) {
    void *ptr;

    assert(nLength > 0);
    ptr = calloc(nLength, nSize);
    if (ptr == NULL)
        throw(EXCEPT_MEM_ALLOC);

    return ptr;
}

void* MemRealloc(void *pOld, size_t nLength, const char *cszPathSrc, const int iLineNo, const char* cszFunc) {
    void *pNew;

    assert(nLength > 0);
    pNew = realloc(pOld, nLength);
    if (pNew == NULL)
        throw(EXCEPT_MEM_ALLOC);

    return pNew;
}

void MemFree(void *ptr) {

    if (ptr != NULL)
        free(ptr);
}

FILE* FileOpen(const char *cszPath, const char *cszMode, const char *cszPathSrc, const int iLineNo, const char* cszFunc) {
    FILE *fptr;

    fptr = fopen(cszPath, cszMode);
    if (fptr == NULL)
        throw(EXCEPT_IO_FILE_OPEN);

    return fptr;
}

size_t FileRead(void *ptr, size_t nSize, size_t nLength, FILE *fptr, const char *cszPathSrc, const int iLineNo, const char *cszFunc) {
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

size_t FileWrite(void *ptr, size_t nSize, size_t nLength, FILE *fptr, const char *cszPathSrc, const int iLineNo, const char *cszFunc) {
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

int FileSeek(FILE *fptr, long iOffset, int iOrigin, const char *cszPathSrc, const int iLineNo, const char *cszFunc) {
    int rc;

    rc = fseek(fptr, iOffset, iOrigin);
    if(rc != 0)
        throw(EXCEPT_IO_FILE_SEEK);

    return rc;
}

int FileClose(FILE *fptr) {
    int rc;

    if (fptr != NULL)
        rc = fclose(fptr);

    return rc;
}


#if defined(_WIN32)

#elif defined(__linux__)
int FileUnlink(const char *cszPathFile, const char *cszPathSrc, const int iLineNo, const char *cszFunc) {
    int rc;

    rc = unlink(cszPathFile);
    if (rc != 0)
        throw(EXCEPT_IO_FILE_UNLINK);

    return 0;
}

int DirMake(const char *cszPathDir, mode_t mode, const char *cszPathSrc, const int iLineNo, const char *cszFunc) {
    int rc;

    rc = mkdir(cszPathDir, mode);
    if (rc != 0) {
        if (errno != EEXIST)
            throw(EXCEPT_IO_DIR_MAKE);
    }

    return rc;
}

DIR* DirOpen(const char *cszPathDir, const char *cszPathSrc, const int iLineNo, const char *cszFunc) {
    DIR *dir;

    dir = NULL;
    if (cszPathDir != NULL) {
        dir = opendir(cszPathDir);
        if (dir == NULL)
            throw(EXCEPT_IO_DIR_OPEN);
    }

    return dir;
}

int DirClose(DIR *dir, const char *cszPathSrc, const int iLineNo, const char *cszFunc) {

    if (dir != NULL)
        closedir(dir);

    return 0;
}

struct dirent* DirRead(DIR *dir, const char *cszPathSrc, const int iLineNo, const char *cszFunc) {
    int oldErrno;
    struct dirent *entry;

    entry = NULL;
    if (dir != NULL) {
        oldErrno = errno;
        entry = readdir(dir);
        if (entry == NULL) {
            if (errno != oldErrno)
                throw(EXCEPT_IO_DIR_READ);
        }
    }

    return entry;
}

FILE* ProcOpen(const char *cszCommand, const char *cszMode, const char *cszPathSrc, const int iLineNo, const char *cszFunc) {
    FILE *fptr;

    fptr = popen(cszCommand, cszMode);
    if (fptr == NULL)
        throw(EXCEPT_PROC_OPEN);

    return fptr;
}

int ProcClose(FILE *fptr, const char *cszPathSrc, const int iLineNo, const char *cszFunc) {
    int rc;

    rc = pclose(fptr);
    if (rc == -1)
        throw(EXCEPT_PROC_CLOSE);

    return 0;
}

void* DLLoad(const char *cszLibName, int mode, const char *cszPathSrc, const int iLineNo, const char *cszFunc) {
    void *hdle;

    hdle = dlopen(cszLibName, mode);
    if (hdle == NULL)
        m_throw(EXCEPT_DL_LOAD, dlerror());

    return hdle;
}

void* DLGetSymbol(void *hdleLib, const char *cszSymbol, const char *cszPathSrc, const int iLineNo, const char *cszFunc) {
    void *hdleSym;

    hdleSym = dlsym(hdleLib, cszSymbol);
    if (hdleSym == NULL)
        m_throw(EXCEPT_DL_GET_SYMBOL, dlerror());

    return hdleSym;
}

void* DLFree(void *hdle, const char *cszPathSrc, const int iLineNo, const char *cszFunc) {

    if (hdle != NULL)
        dlclose(hdle);

    return;
}

#endif
