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

/* Memory related errors. */
#define EXCEPT_MEM              (EXCEPT_NO - 20)
#define EXCEPT_MEM_ALLOC        (EXCEPT_MEM - 1)


/*===========================================================================*
 *                 Implementation of pseudo statements                       *
 *===========================================================================*/
/* Implementation for try statement. */
#define try                 do{\
                                volatile int iExceptId; \
                                iExceptId = setjmp(bufExcept); \
                                switch(iExceptId) { \
                                    case EXCEPT_NO:\

/* Implementation for catch statement. */
#define catch(iExceptId)    break; \
                            case iExceptId: \


/* Implementation for end_try statement. */            
#define end_try             } } while(0)


/* Implementation for throw statement. */
#define throw(iExceptId)    { WriteLog(cszPathFile, iLineNo, cszFunc, "Error Code: %d", GetLastError()); \
                              longjmp(bufExcept, iExceptId); }


/* The buffer stors the destination of long jump when exception occurs. */
extern jmp_buf bufExcept;
            
#endif
