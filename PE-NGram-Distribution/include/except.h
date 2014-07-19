#ifndef _EXCEPT_H
#define _EXCEPT_H

#include "util.h"

/*===========================================================================*
 *                      Definition of error codes                            *
 *===========================================================================*/
/* Base of error codes. */
#define EXCEPT_NO               (0)

/* IO related errors. */
#define EXCEPT_IO               (EXCEPT_NO - 10)
#define EXCEPT_IO_FILE_OPEN     (EXCEPT_IO - 1)
#define EXCEPT_IO_FILE_READ     (EXCEPT_IO - 2)
#define EXCEPT_IO_FILE_WRITE    (EXCEPT_IO - 3)
#define EXCEPT_IO_FILE_SEEK     (EXCEPT_IO - 4)
#define EXCEPT_IO_FILE_UNLINK   (EXCEPT_IO - 5)
#define EXCEPT_IO_DIR_MAKE      (EXCEPT_IO - 6)
#define EXCEPT_IO_DIR_OPEN      (EXCEPT_IO - 7)
#define EXCEPT_IO_DIR_READ      (EXCEPT_IO - 8)

/* Memory related errors. */
#define EXCEPT_MEM              (EXCEPT_NO - 20)
#define EXCEPT_MEM_ALLOC        (EXCEPT_MEM - 1)

/* Process related errors. */
#define EXCEPT_PROC             (EXCEPT_NO - 30)
#define EXCEPT_PROC_OPEN        (EXCEPT_PROC - 1)
#define EXCEPT_PROC_CLOSE       (EXCEPT_PROC - 2)


/*===========================================================================*
 *                 Implementation of pseudo statements                       *
 *===========================================================================*/
/* Implementation for try statement. */
#define try                 do{\
                                volatile int ExceptNum; \
                                ExceptNum = setjmp(bufExcept); \
                                switch(ExceptNum) { \
                                    case EXCEPT_NO:\

/* Implementation for catch statement. */
#define catch(ExceptNum)    break; \
                            case ExceptNum: \


/* Implementation for end_try statement. */            
#define end_try             } } while(0)


/* Implementation for throw statement. */
#if defined(__WIN32)
    #define throw(ExceptNum)    { WriteLog(cszPathSrc, iLineNo, cszFunc, "Error Code: %d\n", GetLastError()); \
                                  longjmp(bufExcept, ExceptNum); }
#elif defined(__linux__)
    #define throw(ExceptNum)    { WriteLog(cszPathSrc, iLineNo, cszFunc, "Message: %s\n", strerror(errno)); \
                                  longjmp(bufExcept, ExceptNum); }
#endif


/* The buffer stors the destination of long jump when exception occurs. */
extern jmp_buf bufExcept;
            
#endif
