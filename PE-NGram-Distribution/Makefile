main: util.o except.o pe_info.o region.o ngram.o main.o 
	gcc -o ngram_distribution *.o -lm

debug: FLAG = -g
debug: util.o except.o pe_info.o region.o ngram.o main.o
	gcc $(FLAG) -o ngram_distribution *.o -lm

main.o: util.h
	gcc $(FLAG) -c main.c

except.o: util.h except.h
	gcc $(FLAG) -c except.c

util.o: util.h except.h
	gcc $(FLAG) -c util.c

pe_info.o: util.h except.h pe_info.h
	gcc $(FLAG) -c pe_info.c

region.o: util.h except.h pe_info.h
	gcc $(FLAG) -c region.c

ngram.o: util.h except.h pe_info.h region.h
	gcc $(FLAG) -c ngram.c

clean:
	rm *.o ngram_distribution

run:
	./ngram_distribution -i $(INPUT) -o $(OUTPUT) -d $(DIMENSION)

run_debug:
	valgrind --leak-check=yes --track-origins=yes \
    ./ngram_distribution -i $(INPUT) -o $(OUTPUT) -d $(DIMENSION)

