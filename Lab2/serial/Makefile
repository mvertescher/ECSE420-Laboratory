program: GEserial.o common.o
	gcc -O0 -o program GEserial.o common.o

GEserial.o: GEserial.c common.h
	gcc -O0 -c GEserial.c

common.o: common.c common.h
	gcc -O0 -c common.c

clean:
	rm *.o program

