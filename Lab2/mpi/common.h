/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : common.h
* ECSE420 -- Parallel Computing
* Creation Date : 11/4/2014
* Created By : Matthew Vertescher 260469756 <matthew.vertescher@mail.mcgill.ca>
_._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._*/

#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdio.h>

struct GEmpi
{
	int N;
	int size;
	int rank;
	double **R;
	int blocking_factor;
	double t_exec;
	double t_proc;
	double t_comm;
};

void print_matrix(double **matrix, int num_rows, int num_cols);
void print_row(double *row, int num_cols);
double **allocate_matrix(int num_rows, int num_cols);
double *allocate_row(int num_cols);
double **initialize_matrix(double **matrix, int num_rows, int num_cols);
double *initialize_row(double *row, int num_cols);
double timer(void);
void write_to_file(char *name, struct GEmpi *param);

#endif