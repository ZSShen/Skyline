PATH_CUR = $(shell pwd)
PATH_ROOT = $(PATH_CUR)/..
PATH_INC = $(PATH_ROOT)/include/
PATH_SRC = $(PATH_CUR)/src/
PATH_OBJ = $(PATH_CUR)/obj/
PATH_LIB = $(PATH_CUR)/lib/

CC = gcc

region: $(PATH_INC)
	$(CC) -c -I$(PATH_INC) $(PATH_SRC)$(NAME) -o $(PATH_OBJ)$@.o
	$(CC) $(PATH_OBJ)$@.o -shared -o $(PATH_LIB)lib$(NAME).so

model: $(PATH_INC)
	$(CC) -c -I$(PATH_INC) $(PATH_SRC)$(NAME) -o $(PATH_OBJ)$@.o
	$(CC) $(PATH_OBJ)$@.o -shared -o $(PATH_LIB)lib$(NAME).so
