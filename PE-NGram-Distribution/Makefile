
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
ifeq ($(DEBUG), true)
	PATH_OUT := $(PATH_OUT_DBG)$(NAME_EXE)
else
	PATH_OUT := $(PATH_OUT_REL)$(NAME_EXE)
endif
PATH_GLD := $(PATH_CUR)/golden/
PATH_GLD_CASE := $(PATH_GLD)case


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
ifeq ($(DEBUG), true)
	FLAG := $(FLAG) -g
endif
ifeq ($(COVERAGE), true)
	FLAG := $(FLAG) -O0 --coverage
endif


# Specify the command to run makefile of the plugin.
MAKE_PLUGIN := make dynamic
ifeq ($(DEBUG), true)
	MAKE_PLUGIN := $(MAKE_PLUGIN) DEBUG=true
endif
ifeq ($(COVERAGE), true)
	MAKE_PLUGIN := $(MAKE_PLUGIN) COVERAGE=true
endif


# List the dependencies for project building.
DEPENDENCY := except util pe_info region ngram report
VPATH := $(PATH_INC)


# List the project building rules.
executable: main
	$(CC) $(FLAG) -o $(PATH_OUT) $(PATH_OBJ)*.o $(LIBS) \
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
ifeq ($(DEBUG), true)
	valgrind --leak-check=yes --track-origins=yes \
    $(PATH_OUT) -i $(INPUT) -o $(OUTPUT) -d $(DIMENSION) -t $(REPORT)
else
	$(PATH_OUT) -i $(INPUT) -o $(OUTPUT) -d $(DIMENSION) -t $(REPORT)
endif


# List the code coverage testing rules.
coverage:
#	tar zxvf $(PATH_GLD_CASE).tar.gz -C $(PATH_GLD)
#	rm -rf $(PATH_GLD_CASE)

