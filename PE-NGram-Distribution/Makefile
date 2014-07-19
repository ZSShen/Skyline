PATH_CUR = $(shell pwd)
PATH_SRC = $(PATH_CUR)/src/
PATH_INC = $(PATH_CUR)/include/
PATH_OBJ = $(PATH_CUR)/obj/
PATH_LIB = $(PATH_CUR)/plugin/lib/

LD_LIBRARY_PATH := $(PATH_LIB)
export LD_LIBRARY_PATH

CC = gcc
DL = -ldl

main: main.o except.o util.o pe_info.o region.o ngram.o report.o
	$(CC) $(DL) -o ngram_distribution $(PATH_OBJ)*.o -lm

debug: FLAG = -g
debug: main.o except.o util.o pe_info.o region.o ngram.o report.o
	$(CC) $(DL) $(FLAG) -o ngram_distribution $(PATH_OBJ)*.o -lm

main.o: $(PATH_INC)
	$(CC) $(FLAG) -I$(PATH_INC) -c $(PATH_SRC)main.c -o $(PATH_OBJ)$@

except.o: $(PATH_INC)
	$(CC) $(FLAG) -I$(PATH_INC) -c $(PATH_SRC)except.c -o $(PATH_OBJ)$@

util.o: $(PATH_INC)
	$(CC) $(FLAG) -I$(PATH_INC) -c $(PATH_SRC)util.c -o $(PATH_OBJ)$@

pe_info.o: $(PATH_INC)
	$(CC) $(FLAG) -I$(PATH_INC) -c $(PATH_SRC)pe_info.c -o $(PATH_OBJ)$@

region.o: $(PATH_INC)
	$(CC) $(FLAG) -I$(PATH_INC) -c $(PATH_SRC)region.c -o $(PATH_OBJ)$@

ngram.o: $(PATH_INC)
	$(CC) $(FLAG) -I$(PATH_INC) -c $(PATH_SRC)ngram.c -o $(PATH_OBJ)$@

report.o: $(PATH_INC)
	$(CC) $(FLAG) -I$(PATH_INC) -c $(PATH_SRC)report.c -o $(PATH_OBJ)$@

clean:
	rm $(PATH_OBJ)*.o ngram_distribution

run:
	./ngram_distribution -i $(INPUT) -o $(OUTPUT) -d $(DIMENSION) -t $(REPORT)

run_debug:
	valgrind --leak-check=yes --track-origins=yes \
    ./ngram_distribution -i $(INPUT) -o $(OUTPUT) -d $(DIMENSION) -t $(REPORT)

