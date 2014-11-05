program: main.o common.o
	gcc -O0 -o program main.o common.o

main.o: main.c common.h
	gcc -O0 -c main.c

common.o: common.c common.h
	gcc -O0 -c common.c

clean:
	rm *.o program

