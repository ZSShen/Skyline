main: util.o except.o pe_info.o region.o ngram.o main.o 
	gcc -o ngram_distribution main.o util.o except.o pe_info.o region.o ngram.o -lm

debug: util.o except.o pe_info.o region.o ngram.o main.o
	gcc -g -o ngram_distribution main.o util.o except.o pe_info.o region.o ngram.o -lm

main.o: util.h
	gcc -c main.c

except.o: util.h except.h
	gcc -c except.c

util.o: util.h except.h
	gcc -c util.c

pe_info.o: util.h except.h pe_info.h
	gcc -c pe_info.c

region.o: util.h except.h pe_info.h
	gcc -c region.c

ngram.o: util.h except.h pe_info.h region.h
	gcc -c ngram.c

clean:
	rm main.o util.o except.o pe_info.o region.o ngram.o ngram_distribution

run:
	./ngram_distribution abc

run_debug:
	valgrind --leak-check=yes --track-origins=yes ./ngram_distribution notepad.exe
