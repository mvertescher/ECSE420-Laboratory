/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : common.c
* ECSE420 -- Parallel Computing
* Creation Date : 11/4/2014
* Created By : Matthew Vertescher 260469756 <matthew.vertescher@mail.mcgill.ca>
_._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._*/

#include "common.h"
#include <sys/time.h>

/**
 *	Prints a matrix A
 */
void print_matrix(double **matrix, int num_rows, int num_cols) 
{
   int r,c;
   for (r = 0; r < num_rows; r++) {
      print_row(matrix[r], num_cols);
   }
}

/**
 *	Prints a single row
 */
void print_row(double *row, int num_cols)
{
  int c;
  for (c = 0; c < num_cols; c++) {
    printf( "%f  ", row[c]);
  }
  printf("\n"); 
}

/**
 *	Prints a single row
 */
double **allocate_matrix(int num_rows, int num_cols)
{
  int r;
  double **matrix;
  matrix = (double **) malloc(num_rows * sizeof(double*));	
  for (r = 0; r < num_rows; r++) {
    matrix[r] = (double *) malloc(num_cols * sizeof(double));
  }
  return matrix;
}

/**
 *	Allocates memory for a single row
 */
double *allocate_row(int num_cols)
{
	return (double *) malloc(num_cols * sizeof(double));
}

/**
 *	Prints a single row
 */
double **initialize_matrix(double **matrix, int num_rows, int num_cols)
{
  int r;
  for (r = 0; r < num_rows; r++) {
    matrix[r] = initialize_row(matrix[r], num_cols);
  }
  return matrix;
}

/**
 *	Initializes a row
 */
double *initialize_row(double *row, int num_cols)
{
  int i;
  for (i = 0; i < num_cols; i++) {
    row[i] =  (double) ((rand() % 100) + 1);
  }  
  return row;
}

/**
 *	Gets the time
 */
double timer(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (((double) tv.tv_usec)/1e6);
}

void write_to_file(char *name, struct GEmpi *param) {
  FILE *fp;
  if (param->rank == 0) {
    fp = fopen(name, "w+");
    fprintf(fp, "N size rank blocking_factor t_exec t_proc t_comm\n");
  } else {
    fp = fopen(name, "a");
  }
  fprintf(fp, "%i %i %i %i %f %f %f \n", param->N, param->size, 
    param->rank, param->blocking_factor,
    param->t_exec, param->t_proc, param->t_comm);
  fclose(fp);
}
