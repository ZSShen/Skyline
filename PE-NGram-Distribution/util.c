#include "util.h"
#include "except.h"

int GenerateErrorMessage(void *szMsg) {
    int iErrCode;
    
    iErrCode = GetLastError();
    FormatMessage( (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS),
                   NULL,
                   iErrCode,
                   MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
                   (LPTSTR)&szMsg,
                   0, NULL);
    return 0;
}

int WriteLog(const char* cszPathFile, int iLineNoNo, const char* cszFunc, const char* cszFormat, ...) 
{
	char		szLogBuf[BUF_SIZE_MID];
	va_list		varArgument;
	int	        iLen;
	SYSTEMTIME	szSysTime;

	memset(szLogBuf, 0, sizeof(char) * BUF_SIZE_MID);
	va_start(varArgument, cszFormat);
	iLen = vsnprintf(szLogBuf, sizeof(szLogBuf), cszFormat, varArgument);
	va_end(varArgument);

	if((iLen == -1) || (iLen >= (int)sizeof(szLogBuf)))
	{
		iLen = sizeof(szLogBuf) - 1;
		szLogBuf[iLen] = 0;
	}
	else if(iLen == 0)
	{
		iLen = 0;
		szLogBuf[0] = 0;
	}

	GetSystemTime(&szSysTime);
    
	printf("%d/%d/%d %d:%d:%d\t%s\n[%s, %d]\n", szSysTime.wYear, szSysTime.wMonth, szSysTime.wDay, szSysTime.wHour, szSysTime.wMinute, szSysTime.wSecond,
												szLogBuf, cszPathFile, iLineNoNo);

	return 0;
}

// Wrapped memory manipulations.
void* MemAlloc(size_t nLength, const char *cszPathFile, const int iLineNo, const char* cszFunc) {
    void *ptr;

    assert(nLength > 0);
	ptr = malloc(nLength);
    if (ptr == NULL)
        throw(EXCEPT_MEM_ALLOC);
    return ptr;
}

void* MemCalloc(size_t nLength, size_t nSize, const char *cszPathFile, const int iLineNo, 
                const char* cszFunc) {
    void *ptr;
    
    assert(nLength > 0);
    ptr = calloc(nLength, nSize);
    if (ptr == NULL)
        throw(EXCEPT_MEM_ALLOC);
    return ptr;
}

void MemFree(void *ptr) {
    if (ptr != NULL)
        free(ptr);
}

// Wrapped file manipulations.
FILE* FileOpen(const char *cszPath, const char *cszMode, const char *cszPathFile, 
                const int iLineNo, const char* cszFunc) {
    FILE *fptr;
    
    fptr = fopen(cszPath, cszMode);
    if (fptr == NULL) {
        throw(EXCEPT_IO_FILE_OPEN);
    }
    
    return fptr;
}

size_t FileRead(void *ptr, size_t nSize, size_t nLength, FILE *fptr, 
                const char *cszPathFile, const int iLineNo, const char *cszFunc) {
    size_t  nRead;
    int     iState;
    
    nRead = fread(ptr, nSize, nLength, fptr);
    if (nRead != (nSize * nLength)) {
        iState = ferror(fptr);
        if (iState != 0) {
            throw(EXCEPT_IO_FILE_READ);
        }
    }
    
    return nRead;
}

size_t FileWrite(void *ptr, size_t nSize, size_t nLength, FILE *fptr,
                 const char *cszPathFile, const int iLineNo, const char *cszFunc) {
    size_t  nWrite;
    int     iState;
    
    nWrite = fwrite(ptr, nSize, nLength, fptr);
    if (nWrite != (nSize * nLength)) {
        iState = ferror(fptr);
        if (iState != 0) {
            throw(EXCEPT_IO_FILE_WRITE);
        }
    }
}


int FileSeek(FILE *fptr, long iOffset, int iOrigin, 
                const char *cszPathFile, const int iLineNo, const char* cszFunc) {
    int iState;
    
    iState = fseek(fptr, iOffset, iOrigin);
    if(iState != 0)
        throw(EXCEPT_IO_FILE_SEEK);
    
    return 0;
}

int FileClose(FILE *fptr) {
    if (fptr != NULL)
        fclose(fptr);
    
    return 0;
}
