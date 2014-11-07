/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : bcast-cons.c
* ECSE420 -- Parallel Computing
* Creation Date : 11/4/2014
* Created By : Matthew Vertescher 260469756 <matthew.vertescher@mail.mcgill.ca>
_._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._*/

#include "../common.h"
#include <mpi.h>

void bcast_cons_run(struct GEmpi *param);
void bcast_cons_get_rows(struct GEmpi *param);
void bcast_cons_distribute_rows(struct GEmpi *param);
void bcast_cons_eliminate(struct GEmpi *param);
void bcast_cons_reduce_rows(struct GEmpi *param);
void bcast_cons_report_result(struct GEmpi *param);

/**
 *	Program begins here
 */
int main(int argc,char** argv)
{
  int N = atoi(argv[1]);
  int sum,size,rank;
  MPI_Status status;
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  struct GEmpi param; 
  param.N = N;
  param.size = size;
  param.rank = rank;
  param.blocking_factor = N / size;
  param.t_exec = 0;
  param.t_proc = 0;
  param.t_comm = 0;
  
  if (N % size == 0) {
  	bcast_cons_run(&param);
  } else {
  	printf("ERROR | blocking_factor is not an integer! \n");
  }

  MPI_Finalize();
}

/**
 *	Method using p2p comm and contiguous row blocks
 */
void bcast_cons_run(struct GEmpi *param)
{
  //printf("N = %i | rank = %i | blocking_factor = %i\n", N, rank, blocking_factor);
  double t1 = timer();
  bcast_cons_get_rows(param);
  bcast_cons_eliminate(param);
  bcast_cons_reduce_rows(param);
  param->t_exec = timer() - t1;
  param->t_proc = param->t_exec - param->t_comm;
  bcast_cons_report_result(param);
} 

/**
 *	Returns the rows the process owns
 */
void bcast_cons_get_rows(struct GEmpi *param) 
{
	param->R = allocate_matrix(param->blocking_factor, param->N); 

  if (param->rank == 0) {
    bcast_cons_distribute_rows(param);
  } else {
  	MPI_Status status;
  	int r;
    double t1;
    for (r = 0; r < param->blocking_factor; r++) {
      t1 = timer(); 
      MPI_Recv(param->R[r], param->N, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD,&status);
      param->t_comm += timer() - t1;
      //printf("Rank %i | recieved row\n", rank);
    }
  }
}

/**
 *	Used only by Rank 0 to distribute the rows to the other processes
 */
void bcast_cons_distribute_rows(struct GEmpi *param) 
{
	int N = param->N;
  int size = param->size;  
  int rank = param->rank; 
  double **R = param->R;
  int blocking_factor = param->blocking_factor;

  int r, cur_rank;
  //printf("\nPrinting input matrix:\n");

  // Allocate Rank 0 rows
  for (r = 0; r < blocking_factor; r++) {
    R[r] = initialize_row(R[r], N);
    //print_row(R[r], N);
  }

  // Send to procs with blocking_factor rows
  double *buffer = allocate_row(N);
  double t1;
  for (cur_rank = 1; cur_rank < size; cur_rank++) {
    for (r = 0; r < blocking_factor; r++) {
      buffer = initialize_row(buffer, N);
      //print_row(buffer, N);
      //printf("Root about to send row to (rank = %i)\n", cur_rank);
      t1 = timer();
      MPI_Send(buffer, N, MPI_DOUBLE, cur_rank, 1, MPI_COMM_WORLD);
      param->t_comm += timer() - t1;
    }
  }
  free(buffer);
}

/**
 *	Parrallelized gaussian elimination
 */
void bcast_cons_eliminate(struct GEmpi *param) {
 	int N = param->N;
  int size = param->size;  
  int rank = param->rank; 
  double **R = param->R;
  int blocking_factor = param->blocking_factor;

  MPI_Status status;
  int pivot = 0;
  int cur_rank,r,row,col;
  double factor, t1;
  double buffer[N];

  // For every lower rank
  for (cur_rank = 0; cur_rank < rank; cur_rank++) {
    // Iterate through the rows sent by the current rank
    for (r = 0; r < blocking_factor; r++) {
      // Receive the row
      t1 = timer();
      //MPI_Recv(&buffer[pivot], N - pivot, MPI_DOUBLE, cur_rank, 1, MPI_COMM_WORLD,&status);
      MPI_Bcast(&buffer[pivot], N - pivot, MPI_DOUBLE, cur_rank, MPI_COMM_WORLD);
      param->t_comm += timer() - t1;
      // Eliminate the buffered row to all rows this proc owns
      for (row = 0; row < blocking_factor; row++) {
        factor = R[row][pivot] / buffer[pivot];
        for (col = pivot; col < N; col++) { 
          R[row][col] = R[row][col] - factor * buffer[col];
        }
      }
      pivot++;
    }
  }

  /* // Eliminate owned rows, standard GE
  for (row = 0; row < blocking_factor - 1; row++) {
    for (r = row + 1; r < blocking_factor; r++) {
      factor = R[r][row + pivot] / R[row][row + pivot];
      for (col = 0; col < N; col++)
        R[r][col] = R[r][col] - factor * R[row][col];
    }
  } */

  // For every row owned
  for (row = 0; row < blocking_factor; row++) {
    t1 = timer();
    //MPI_Send(&R[row][pivot], N - pivot, MPI_DOUBLE, cur_rank, 1, MPI_COMM_WORLD);
    MPI_Bcast(&R[row][pivot], N - pivot, MPI_DOUBLE, rank, MPI_COMM_WORLD);
    param->t_comm += timer() - t1;
    if (row == blocking_factor - 1 ) { break; }
    // Eliminate owned rows, standard GE
    for (r = row + 1; r < blocking_factor; r++) {
      factor = R[r][pivot] / R[row][pivot];
      for (col = 0; col < N; col++)
        R[r][col] = R[r][col] - factor * R[row][col];
    }
    pivot++;
  }

  // Listen for excess broadcasts 
  for (cur_rank = rank + 1; cur_rank < size; cur_rank++) {
    for (r = 0; r < blocking_factor; r++) {
      t1 = timer();
      MPI_Bcast(&buffer[pivot], N - pivot, MPI_DOUBLE, cur_rank, MPI_COMM_WORLD);
      param->t_comm += timer() - t1;
      pivot++;
    }
  }
}

/**
 *	Reduce the rows back into a matrix
 */
void bcast_cons_reduce_rows(struct GEmpi *param)
{
  int N = param->N;
  int size = param->size;  
  int rank = param->rank; 
  double **R = param->R;
  int blocking_factor = param->blocking_factor;

	MPI_Status status;
	int buf;
  double t1;

  if (rank == 0) {
    //printf("\n\nPrinting output matrix:\n");
    //print_matrix(R, blocking_factor, N);

    int cur_rank, buf;
    for (cur_rank = 1; cur_rank < size; cur_rank++) {
      t1 = timer();
      MPI_Send(&buf, 1, MPI_INT, cur_rank, 1, MPI_COMM_WORLD);
      MPI_Recv(&buf, 1, MPI_INT, cur_rank, 1, MPI_COMM_WORLD, &status);
      param->t_comm += timer() - t1;
    }
  } else {
    t1 = timer();
    MPI_Recv(&buf, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    //print_matrix(R, blocking_factor, N);
    MPI_Send(&buf, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
    param->t_comm += timer() - t1;
  }
}

/**
 *  Print the results to a file 
 */
void bcast_cons_report_result(struct GEmpi *param) 
{
  //printf("rank = %i | blocking_factor = %i | t_exec = %f | t_proc = %f | t_comm = %f \n", param->rank, param->blocking_factor, param->t_exec, param->t_proc, param->t_comm); 
  
  int N = param->N;
  int size = param->size;  
  int rank = param->rank; 
  free(param->R);
  int blocking_factor = param->blocking_factor;

  MPI_Status status;
  int buf;

  char file_name[50];
  sprintf(file_name,"./results/bcast-cons-%i-%i.dat",param->N,param->size);

  if (rank == 0) {
    write_to_file(file_name, param);
    int cur_rank, buf;
    for (cur_rank = 1; cur_rank < size; cur_rank++) {
      MPI_Send(&buf, 1, MPI_INT, cur_rank, 1, MPI_COMM_WORLD);
      MPI_Recv(&buf, 1, MPI_INT, cur_rank, 1, MPI_COMM_WORLD, &status);
    }
    printf("%f %f %f\n", param->t_exec, param->t_proc, param->t_comm); 
  } else {
    MPI_Recv(&buf, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    write_to_file(file_name, param);
    MPI_Send(&buf, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
  }

}

