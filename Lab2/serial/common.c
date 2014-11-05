/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : common.c
* ECSE420 -- Parallel Computing
* Creation Date : 11/4/2014
* Created By : Matthew Vertescher 260469756 <matthew.vertescher@mail.mcgill.ca>
_._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._*/


#include "common.h"
#include <sys/time.h>
#include <string.h>

double **allocateMatrix(int x, int y)
{
  int i;
  double **matrix;
  matrix = (double **) malloc(x * sizeof(double*));
  for (i = 0; i < x; i++)
    matrix[i] = (double *) malloc( y * sizeof(double));

  return matrix;
}


double **initializeMatrix(double **matrix, int x, int y)
{
  int i,j;
  for (i = 0; i < x; i++)
    for (j = 0; j < y; j++)
	     matrix[i][j] = (rand() % 100) + 1;
  return matrix;
}


void printMatrix(double **matrix, int x, int y)
{
  int i,j;
  
  printf("\n\n");
  for (i = 0; i < x; i++) {
    for (j = 0; j < y; j++) {
  	printf( "%f  ", matrix[i][j] );
    }
    printf("\n");
  }

}


double timer(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (((double) tv.tv_usec)/1e6);
}

