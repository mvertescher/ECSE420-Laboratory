/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : main.c
* ECSE420  -- Parallel Computing
* Creation Date : 10/09/2014
* Created By : Dimitrios Stamoulis  <dimitrios.stamoulis@mail.mcgill.ca>
* Modified By : Matthew Vertescher 260469756
_._._._._._._._._._._._._._._._._._._._._.*/


#include <stdio.h>
#include <stdlib.h>
#include "common.h"

void child(int i, int **A, int B, int x, int color, int *fd1, int *fd2);
void father();
void readPipe(int *fd, int i);
void writePipe(int *fd, int i);

int main()
{

  int **A;
  int i,j, done = 0;
  double begSer, endSer;


  // this is the color to be filtered. Comment out accordingly 
  //
  int color = 1; // red
  // int color = 2; // green
  // int color = 3; // yellow
  // int color = 4; // blue
  // int color = 5; // magenta
  // int color = 6; // cyan



  // initialization phase done by the main process
  // you could define different matrix sizes inside the 
  // "common.h" header file. 
  A = allocateMatrix(X, Y);
  A = initializeMatrix(A, X, Y);
  //printImage(A,0,X-1,0,Y-1);
  printf("\nInitial Image.\n\n\n");


  // filtering the image -- serial way
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //begSer = timer();
  A = colorFilter(A,0,X-1,0,Y-1,color);
  printImage(A,0,X-1,0,Y-1);
  //endSer = timer();

  // tools for printing the overall execution time
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //printf("\nX = %i  Y = %i  NUM_PROCS = %i ",X,Y,NUM_PROCS);
  //printf("\nSerial -- Execution time : %.10e\n\n\n", endSer - begSer );  

  //return 0; 

  // filtering the image -- parallel
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // TODO: 
  //   
  // 1) The main process will remain responsible for
  // initializing the parallelization process and then 
  // synchronizing everything. 
  //
  // 2) The computation part will be done by the children processes.
  // Once they are done, children should print the outcome of their 
  // execution (their portion of matrix that they were assigned).
  
  /***
  
  Here is where my parallel version should be placed.

  ***/

  printf("\n\n");

  // Inits
  int p;
  double begPar, endPar;
  
  // Determine the number of rows for each process to
  int B = X / NUM_PROCS;
  int offset = 0;
  
  // Create two pipes 
  int fd1[2]; 
  int fd2[2];  
  pipe(fd1);
  pipe(fd2);

  begPar = timer();

  // The parent should fork NUM_PROCS children
  for (i = 0; i < NUM_PROCS; i++) {
    p = fork();
    if (p == 0) {
      child(i, A, B, offset, color, fd1, fd2);
    } else {
      offset += B; // The father just increments the row offset 
    }
  }

  close(fd1[0]); // For parent to write
  close(fd2[1]); // For parent to read

  // Once all the children have been forked the parent should 
  // wait for them to finish processing using pipes
  for (i = 0; i < NUM_PROCS; i++) {
    writePipe(fd1, (i + 1));
    readPipe(fd2, (i + 1));
  }

  endPar = timer();

  printf("\nX = %i  Y = %i  NUM_PROCS = %i ",X,Y,NUM_PROCS);
  printf("\nParallel -- Execution time : %.10e\n\n\n", endPar - begPar );  

  return 0; 

}

void child(int i, int **A, int B, int x, int color, int *fd1, int *fd2) {

  //close(fd1[1]); // Child read
  close(fd2[0]); // Child write
  
  int x2 = x + B - 1;
  if ((i + 1) == NUM_PROCS) {
    x2 = X - 1;
  }

  colorFilter(A,x,x2,0,Y-1,color);

  //readPipe(fd1, (i + 1));
  printImage(A,x,x2,0,Y-1);
  //printf("Child %i is printing! \n", i+1);
  //writePipe(fd2, (i + 1));

  _exit(EXIT_SUCCESS);
}

void readPipe(int *fd, int i) {
  int buffer[1];
  buffer[0] = 0;

  while (1) {
    read(fd[0], buffer, sizeof(int));
    if (buffer[0] == i) { break; }
    else { 
      write(fd[1], buffer, sizeof(buffer)); 
    }
  }
}

void writePipe(int *fd, int i) {
  int writebuffer[1];
  writebuffer[0] = i;
  write(fd[1], writebuffer, sizeof(int));
}

