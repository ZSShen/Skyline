
# Define the constants for folders and files.
PATH_CUR := $(shell pwd)
PATH_SRC := $(PATH_CUR)/src/
PATH_INC := $(PATH_CUR)/include/
PATH_OBJ := $(PATH_CUR)/obj/
PATH_PLG := $(PATH_CUR)/plugin/
PATH_PLG_OBJ := $(PATH_PLG)obj/
PATH_PLG_LIB := $(PATH_PLG)lib/
PATH_OUT_DBG := $(PATH_CUR)/debug/
PATH_OUT_REL := $(PATH_CUR)/release/
NAME_EXE := pe_ngram
NAME_LIB := lib$(NAME_EXE)


# Create the relevant folders to store objective and executable files.
FOLDER_OBJ := $(shell mkdir -p $(PATH_OBJ))
FOLDER_OUT_DBG := $(shell mkdir -p $(PATH_OUT_DBG))
FOLDER_OUT_REL := $(shell mkdir -p $(PATH_OUT_REL))


# Export the LD_LIBRARY_PATH environment variable for shared object utilization.
LD_LIBRARY_PATH := $(PATH_PLG_LIB)
export LD_LIBRARY_PATH


# Specify the compilation options.
CC := gcc
LIBS := -ldl -lm
FLAG := -fPIC


# Specify the command to run makefile of the plugin.
MAKE_PLUGIN := make dynamic


# List the dependencies for project building.
DEPENDENCY := except util pe_info region ngram report
VPATH := $(PATH_INC)


# List the project building rules.
release_exe: main
	$(CC) -o $(PATH_OUT_REL)$(NAME_EXE) $(PATH_OBJ)*.o $(LIBS) \
    && cd $(PATH_PLG) \
    && $(MAKE_PLUGIN)

debug_exe: FLAG := $(FLAG) -g
debug_exe: main
	$(CC) $(FLAG) -o $(PATH_OUT_DBG)$(NAME_EXE) $(PATH_OBJ)*.o $(LIBS) \
    && cd $(PATH_PLG) \
    && $(MAKE_PLUGIN)

main: $(DEPENDENCY)
	$(CC) $(FLAG) -I$(PATH_INC) -c $(PATH_SRC)$@.c -o $(PATH_OBJ)$@.o

$(DEPENDENCY):
	$(CC) $(FLAG) -I$(PATH_INC) -c $(PATH_SRC)$@.c -o $(PATH_OBJ)$@.o


# List the project cleaning rule.
.PHONY: clean
clean:
	rm -f $(PATH_OBJ)* $(PATH_OUT_DBG)* $(PATH_OUT_REL)* \
          $(PATH_PLG_OBJ)* $(PATH_PLG_LIB)*


# List the project execution rules.
run:
	$(PATH_OUT_REL)$(NAME_EXE) -i $(INPUT) -o $(OUTPUT) -d $(DIMENSION) -t $(REPORT)

debug_run:
	valgrind --leak-check=yes --track-origins=yes \
    $(PATH_OUT_DBG)$(NAME_EXE) -i $(INPUT) -o $(OUTPUT) -d $(DIMENSION) -t $(REPORT)

