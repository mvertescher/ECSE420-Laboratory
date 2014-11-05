/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : common.c
* ECSE420  -- Parallel Computing
* Creation Date : 10/09/2014
* Created By : Dimitrios Stamoulis  <dimitrios.stamoulis@mail.mcgill.ca>
_._._._._._._._._._._._._._._._._._._._._.*/


#include "common.h"
#include <sys/time.h>
#include <string.h>

int ** allocateMatrix( int x, int y )
{
  int i;
  int **matrix;
  matrix = (int **) malloc(x * sizeof(int*));
  for (i=0;i<x;i++)
    matrix[i] = (int *) malloc( y * sizeof(int));

  return matrix;
}


int **initializeMatrix(int **matrix, int x, int y)
{
  int i,j;

  // initialize my image with random colors   
  for (i=0;i<x;i++)
    for (j=0;j<y;j++)
	matrix[i][j] = rand()%6+1;

  return matrix;
}



void printMatrix( double **matrix, int x, int y)
{
  int i,j;
  
  printf("\n\n");
  for (i=0;i<x;i++)
  {
    for (j=0;j<y;j++)
    {
  	printf( "%f  ", matrix[i][j] );
    }
    printf("\n");
  }

}


void printColor(int pixel){

  switch (pixel)
  {

      case 0:
      printf(ANSI_COLOR_BLACK   "@"  ANSI_COLOR_RESET ); break;
      case 1:
      printf(ANSI_COLOR_RED     "@"  ANSI_COLOR_RESET ); break;
      case 2:
      printf(ANSI_COLOR_GREEN   "@"  ANSI_COLOR_RESET ); break;
      case 3:
      printf(ANSI_COLOR_YELLOW  "@"  ANSI_COLOR_RESET ); break;
      case 4:
      printf(ANSI_COLOR_BLUE    "@"  ANSI_COLOR_RESET ); break;
      case 5:
      printf(ANSI_COLOR_MAGENTA "@"  ANSI_COLOR_RESET ); break;
      case 6:
      printf(ANSI_COLOR_CYAN    "@"  ANSI_COLOR_RESET ); break;
  }

}



void printImage( int **matrix, int x_start, int x_end, int y_start, int y_end)
{
  int i,j;
  
  for (i=x_start;i<=x_end;i++)
  {
    for (j=y_start;j<=y_end;j++)
    {
  	printColor(matrix[i][j]);
    }
    printf("\n");
  }

}



int **colorFilter(int **matrix, int x_start, int x_end, int y_start, int y_end, int color)
{
  int i,j;

  // let's apply the filter operator to the portion of image that is defined 
  // based on the (x_start, x_end, y_start, y_end) arguments.   
  for (i=x_start;i<=x_end;i++)
    for (j=y_start;j<=y_end;j++)
        if (matrix[i][j] != color)
	  matrix[i][j] = 0;

  return matrix;
}



double timer(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (((double) tv.tv_usec)/1e6);
}





