/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : common.h
* ECSE420  -- Parallel Computing
* Creation Date : 10/30/2014
* Created By : Matthew Vertescher 260469756 <matthew.vertescher@mail.mcgill.ca>
_._._._._._._._._._._._._._._._._._._._._.*/

#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdio.h>

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

double **allocateMatrix(int x, int y);
double **initializeMatrix(double **matrix, int x, int y);
void printMatrix(double **matrix, int x, int y);
double timer(void);

#endif

