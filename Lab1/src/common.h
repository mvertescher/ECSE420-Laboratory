/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : common.h
* ECSE420  -- Parallel Computing
* Creation Date : 10/09/2014
* Created By : Dimitrios Stamoulis  <dimitrios.stamoulis@mail.mcgill.ca>
_._._._._._._._._._._._._._._._._._._._._.*/

#ifndef COMMON_H
#define COMMON_H

#define X 10 // 10  
#define Y 100 // 80
#define NUM_PROCS 10

#include <stdlib.h>
#include <stdio.h>

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>


#define ANSI_COLOR_BLACK   "\x1b[30m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"



int **allocateMatrix(int x, int y);
int **initializeMatrix(int **matrix, int x, int y);
int **colorFilter(int **matrix, int x_start, int x_end, int y_start, int y_end, int color);
void printMatrix(int **matrix, int x, int y);
void printColor(int pixel);
void printImage(int **matrix, int x_start, int x_end, int y_start, int y_end);
double timer(void);

#endif

