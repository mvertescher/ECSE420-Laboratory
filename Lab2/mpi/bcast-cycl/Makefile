program: bcast-cycl.o ../common.o
	mpicc -O0 -o program bcast-cycl.o ../common.o

bcast-cycl.o: bcast-cycl.c ../common.h
	mpicc -O0 -c bcast-cycl.c

common.o: ../common.c ../common.h
	mpicc -O0 -c ../common.c

clean:
	rm *.o program

