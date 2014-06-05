main: main.o util.o except.o pe_info.o policy.o report.o debug.o
	gcc -o main.exe main.o report.o policy.o pe_info.o except.o util.o debug.o

#main_build: main.o util.o except.o pe_info.o policy.o debug.o
#	gcc -o main.exe main.o util.o except.o pe_info.o policy.o debug.o
    
main.o: main.c util.h pe_info.h
	gcc -c main.c
    
util.o: util.c util.h
	gcc -c util.c
    
except.o: except.c util.h except.h
	gcc -c except.c
    
pe_info.o: pe_info.c util.h except.h pe_info.h
	gcc -c pe_info.c

debug.o: debug.c util.h except.h pe_info.h
	gcc -c debug.c    

policy.o: policy.c util.h except.h pe_info.h policy.h
	gcc -c policy.c

report.o: report.c util.h except.h pe_info.h report.h    
	gcc -c report.c
    
clean:
	del main.o util.o except.o pe_info.o policy.o report.o main.exe
	
run:
	main.exe
	