PATH_CUR = $(shell pwd)
PATH_ROOT = $(PATH_CUR)/..
PATH_INC = $(PATH_ROOT)/include/
PATH_SRC = $(PATH_CUR)/src/
PATH_OBJ = $(PATH_CUR)/obj/
PATH_LIB = $(PATH_CUR)/lib/

PATH_FILE_EXCEPT = $(PATH_ROOT)/obj/except.o
PATH_FILE_UTIL = $(PATH_ROOT)/obj/util.o
PATH_FILE_PE = $(PATH_ROOT)/obj/pe_info.o
PATH_FILE_NGRAM = $(PATH_ROOT)/obj/ngram.o

CC = gcc

region: $(PATH_INC)
	$(CC) -c -I$(PATH_INC) $(PATH_SRC)$(NAME).c -o $(PATH_OBJ)$@.o
	$(CC) $(PATH_FILE_EXCEPT) $(PATH_FILE_UTIL) $(PATH_FILE_PE) $(PATH_OBJ)$@.o -fPIC -shared -o $(PATH_LIB)lib$(NAME).so	

model: $(PATH_INC)
	$(CC) -c -I$(PATH_INC) $(PATH_SRC)$(NAME).c -o $(PATH_OBJ)$@.o
	$(CC) $(PATH_FILE_EXCEPT) $(PATH_FILE_UTIL) $(PATH_FILE_NGRAM) $(PATH_OBJ)$@.o -fPIC -shared -o $(PATH_LIB)lib$(NAME).so
