
# Define the constants.
PATH_CUR := $(shell pwd)
PATH_SRC := $(PATH_CUR)/src/
PATH_INC := $(PATH_CUR)/include/
PATH_OBJ := $(PATH_CUR)/obj/
PATH_LIB := $(PATH_CUR)/plugin/lib/
PATH_OUT_DBG := $(PATH_CUR)/debug/
PATH_OUT_REL := $(PATH_CUR)/release/

LD_LIBRARY_PATH := $(PATH_LIB)
export LD_LIBRARY_PATH

CC := gcc
LIBS := -ldl -lm

NAME_EXE := pe_ngram
NAME_LIB := lib$(NAME_EXE)

VPATH := $(PATH_INC)


# Create the relevant folders to store objective and executable files.
FOLDER_OBJ := $(shell mkdir -p $(PATH_OBJ))
FOLDER_LIB := $(shell mkdir -p $(PATH_LIB))
FOLDER_OUT_DBG := $(shell mkdir -p $(PATH_OUT_DBG))
FOLDER_OUT_REL := $(shell mkdir -p $(PATH_OUT_REL))


# List the dependencies for project building.
DEPENDENCY := except util pe_info region ngram report


# List the project building rules.
release_exe: main
	$(CC) -o $(PATH_OUT_REL)$(NAME_EXE) $(PATH_OBJ)*.o $(LIBS)

debug_exe: FLAG := -g
debug_exe: main
	$(CC) $(FLAG) -o $(PATH_OUT_REL)$(NAME_EXE) $(PATH_OBJ)*.o $(LIBS)

main: $(DEPENDENCY)
	$(CC) $(FLAG) -I$(PATH_INC) -c $(PATH_SRC)$@.c -o $(PATH_OBJ)$@.o

$(DEPENDENCY):
	$(CC) $(FLAG) -I$(PATH_INC) -c $(PATH_SRC)$@.c -o $(PATH_OBJ)$@.o


# List the project cleaning rule.
.PHONY: clean
clean:
	rm -f $(PATH_OBJ)*.o $(PATH_OUT_DBG)*.o $(PATH_OUT_REL)*.o

#run:
#	./ngram_distribution -i $(INPUT) -o $(OUTPUT) -d $(DIMENSION) -t $(REPORT)

#run_debug:
#	valgrind --leak-check=yes --track-origins=yes \
#    ./ngram_distribution -i $(INPUT) -o $(OUTPUT) -d $(DIMENSION) -t $(REPORT)

