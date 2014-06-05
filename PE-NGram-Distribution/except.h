#ifndef _EXCEPT_H
#define _EXCEPT_H

#include "util.h"

#define EXCEPT_NO               (0)

#define EXCEPT_IO               (EXCEPT_NO - 10)
#define EXCEPT_IO_FILE_OPEN     (EXCEPT_IO - 1)
#define EXCEPT_IO_FILE_READ     (EXCEPT_IO - 2)
#define EXCEPT_IO_FILE_WRITE    (EXCEPT_IO - 3)
#define EXCEPT_IO_FILE_SEEK     (EXCEPT_IO - 4)

#define EXCEPT_MEM              (EXCEPT_NO - 20)
#define EXCEPT_MEM_ALLOC        (EXCEPT_MEM - 1)

#define try                 do{\
                                volatile int iExceptId; \
                                iExceptId = setjmp(bufExcept); \
                                switch(iExceptId) { \
                                    case EXCEPT_NO:\

#define catch(iExceptId)    break; \
                            case iExceptId: \
            
#define end_try             } } while(0)

#define throw(iExceptId)    { WriteLog(cszPathFile, iLineNo, cszFunc, "Error Code: %d", GetLastError()); \
                              longjmp(bufExcept, iExceptId); }


extern jmp_buf bufExcept;
            
#endif