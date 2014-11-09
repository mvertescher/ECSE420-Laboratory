/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : GEserial.c
* ECSE420  -- Parallel Computing
* Creation Date : 10/30/2014
* Created By : Matthew Vertescher 260469756 <matthew.vertescher@mail.mcgill.ca>
_._._._._._._._._._._._._._._._._._._._._.*/


#include <stdio.h>
#include <stdlib.h>
#include "common.h"

int main(int argc,char** argv)
{
  //printf("GEserial.c | main() \n");
  int N = atoi(argv[1]);

  double **A;
  int i,j,k;
  double l;
  double begSer, endSer, t_exec;

  begSer = timer(); 

  A = allocateMatrix(N,N);
  A = initializeMatrix(A,N,N);

  // Print the initial matrix
  //printMatrix(A, X, Y);

  for (k = 0; k < N - 1; k++) {
    for (i = k + 1; i < N; i++) {
      l = A[i][k] / A[k][k];
      for (j = 0; j < N; j++) 
        A[i][j] = A[i][j] - l * A[k][j];
    }
  }

  // Print the matrix in triangular form 
  //printMatrix(A, X, Y);

  endSer = timer();
  t_exec = endSer - begSer;

  free(A);
  
  //printf("Serial N = %i  t_exec = %.10e \n", N, t_exec);  
  printf("%f\n", t_exec);

  return 0; 

}
