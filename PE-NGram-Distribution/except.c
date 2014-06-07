#include "except.h"
#include "util.h"

/* The buffer stors the destination of long jump when exception occurs. */
jmp_buf bufExcept;
