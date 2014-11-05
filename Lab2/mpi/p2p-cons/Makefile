program: p2p-cons.o ../common.o
	mpicc -O0 -o program p2p-cons.o ../common.o

p2p-cons.o: p2p-cons.c ../common.h
	mpicc -O0 -c p2p-cons.c

common.o: ../common.c ../common.h
	mpicc -O0 -c ../common.c

clean:
	rm *.o program

