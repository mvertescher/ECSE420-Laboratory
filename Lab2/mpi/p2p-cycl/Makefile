program: p2p-cycl.o ../common.o
	mpicc -O0 -o program p2p-cycl.o ../common.o

p2p-cycl.o: p2p-cycl.c ../common.h
	mpicc -O0 -c p2p-cycl.c

common.o: ../common.c ../common.h
	mpicc -O0 -c ../common.c

clean:
	rm *.o program

