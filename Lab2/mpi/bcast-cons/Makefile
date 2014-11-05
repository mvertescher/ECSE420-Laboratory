program: bcast-cons.o ../common.o
	mpicc -O0 -o program bcast-cons.o ../common.o

bcast-cons.o: bcast-cons.c ../common.h
	mpicc -O0 -c bcast-cons.c

common.o: ../common.c ../common.h
	mpicc -O0 -c ../common.c

clean:
	rm *.o program

