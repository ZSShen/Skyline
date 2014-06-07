main: util.o except.o main.o 
	gcc -o ngram_distribution main.o util.o except.o

main.o: util.h
	gcc -c main.c

except.o: util.h except.h
	gcc -c except.c

util.o: util.h except.h
	gcc -c util.c

pe_info.o: util.h except.h pe_info.h
	gcc -c pe_info.c

clean:
	rm main.o util.o except.o ngram_distribution
