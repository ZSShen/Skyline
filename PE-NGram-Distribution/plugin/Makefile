
# Define the constants for folders and files.
PATH_CUR := $(shell pwd)
PATH_PRJ := $(PATH_CUR)/..
PATH_INC := $(PATH_PRJ)/include/
PATH_OBJ := $(PATH_PRJ)/obj/
PATH_PLG_SRC := $(PATH_CUR)/src/
PATH_PLG_OBJ := $(PATH_CUR)/obj/
PATH_PLG_LIB := $(PATH_CUR)/lib/

PATH_OBJ_EXCEPT := $(PATH_OBJ)except.o
PATH_OBJ_UTIL   := $(PATH_OBJ)util.o
PATH_OBJ_PE     := $(PATH_OBJ)pe_info.o
PATH_OBJ_NGRAM  := $(PATH_OBJ)ngram.o


# Define the default plugins.
REGION := Region_MaxEntropySection
MODEL  := Model_DescendingFrequency
PATH_REGION := $(PATH_PLG_SRC)$(REGION)
PATH_MODEL  := $(PATH_PLG_SRC)$(MODEL)


# Create the relevant folders to store objective and executable files.
FOLDER_OBJ := $(shell mkdir -p $(PATH_PLG_OBJ))
FOLDER_LIB := $(shell mkdir -p $(PATH_PLG_LIB))


# Specify the compilation options
CC = gcc
FLAG = -fPIC
ifeq ($(DEBUG), true)
	FLAG := $(FLAG) -g
endif


# List the dependencies for project building.
VPATH := $(PATH_INC)

# List the project building rules.
dynamic: clean dynamic_region dynamic_model  

dynamic_region:
	$(CC) $(FLAG) -I$(PATH_INC) -c $(PATH_REGION).c -o $(PATH_OBJ)$@.o	
	$(CC) $(FLAG) -shared $(PATH_OBJ_EXCEPT) $(PATH_OBJ_UTIL) $(PATH_OBJ_PE) \
    $(PATH_OBJ)$@.o -o $(PATH_PLG_LIB)lib$(REGION).so

dynamic_model:
	$(CC) $(FLAG) -I$(PATH_INC) -c $(PATH_MODEL).c -o $(PATH_OBJ)$@.o
	$(CC) $(FLAG) -shared $(PATH_OBJ_EXCEPT) $(PATH_OBJ_UTIL) $(PATH_OBJ_NGRAM) \
    $(PATH_OBJ)$@.o -o $(PATH_PLG_LIB)lib$(MODEL).so


# List the project cleaning rule.
clean:
	rm -f $(PATH_PLG_OBJ)* $(PATH_PLG_LIB)*
