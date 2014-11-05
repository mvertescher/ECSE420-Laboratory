/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : bcast-cycl.c
* ECSE420 -- Parallel Computing
* Creation Date : 11/4/2014
* Created By : Matthew Vertescher 260469756 <matthew.vertescher@mail.mcgill.ca>
_._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._._*/

#include "../common.h"
#include <mpi.h>

void bcast_cycl_run(struct GEmpi *param);
void bcast_cycl_get_rows(struct GEmpi *param);
void bcast_cycl_distribute_rows(struct GEmpi *param);
void bcast_cycl_eliminate(struct GEmpi *param);
void bcast_cycl_reduce_rows(struct GEmpi *param);
void bcast_cycl_report_result(struct GEmpi *param);

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
  	bcast_cycl_run(&param);
  } else {
  	printf("ERROR | blocking_factor is not an integer! \n");
  }

  MPI_Finalize();
}

/**
 *	Method using p2p comm and contiguous row blocks
 */
void bcast_cycl_run(struct GEmpi *param)
{
  //printf("N = %i | rank = %i | blocking_factor = %i\n", N, rank, blocking_factor);
  double t1 = timer();
  bcast_cycl_get_rows(param);
  bcast_cycl_eliminate(param);
  bcast_cycl_reduce_rows(param);
  param->t_exec = timer() - t1;
  param->t_proc = param->t_exec - param->t_comm;
  bcast_cycl_report_result(param);
} 

/**
 *	Returns the rows the process owns
 */
void bcast_cycl_get_rows(struct GEmpi *param) 
{
	param->R = allocate_matrix(param->blocking_factor, param->N); 

  if (param->rank == 0) {
    bcast_cycl_distribute_rows(param);
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
void bcast_cycl_distribute_rows(struct GEmpi *param) 
{
	int N = param->N;
  int size = param->size;  
  int rank = param->rank; 
  double **R = param->R;
  int blocking_factor = param->blocking_factor;

  int row, cur_rank;
  int r = 0;
  //printf("\nPrinting input matrix:\n");

  double *buffer;
  double t1;

  // Cyclic distribution of blocks
  for (row = 0; row < N; row++) {
    cur_rank = row % size; 
    if (cur_rank == 0) {
      R[r] = initialize_row(R[r], N);
      //print_row(R[r], N);
      r++;
    } else {
      buffer = allocate_row(N);
      buffer = initialize_row(buffer, N);
      //printf(" RANK = %i  ",cur_rank); 
      //print_row(buffer, N);
      t1 = timer();
      MPI_Send(buffer, N, MPI_DOUBLE, cur_rank, 1, MPI_COMM_WORLD);
      param->t_comm += timer() - t1;
    }
    cur_rank++;
  }

}

/**
 *	Parrallelized gaussian elimination
 */
void bcast_cycl_eliminate(struct GEmpi *param) {
 	int N = param->N;
  int size = param->size;  
  int rank = param->rank; 
  double **R = param->R;
  int blocking_factor = param->blocking_factor;

  MPI_Status status;
  int pivot;
  int row_index = 0;
  int cur_rank,row,col,r;
  double factor, t1;
  double buffer[N];

  // For every row pivot
  for (pivot = 0; pivot < N; pivot++) {
    cur_rank = pivot % size;
    // If the current process owns the row
    if (cur_rank == rank) {
      t1 = timer();
      //MPI_Send(&R[row_index][pivot], N - pivot, MPI_DOUBLE, r, 1, MPI_COMM_WORLD);
      MPI_Bcast(&R[row_index][pivot], N - pivot, MPI_DOUBLE, rank, MPI_COMM_WORLD);
      //MPI_Barrier(MPI_COMM_WORLD);
      param->t_comm += timer() - t1;

      // Eliminate the row in the current process
      for (row = row_index + 1; row < blocking_factor; row++) {
        factor = R[row][pivot] / R[row_index][pivot];
        for (col = pivot; col < N; col++) { 
          R[row][col] = R[row][col] - factor * R[row_index][col];
        }
      }

      row_index++; /* This row is now fully eliminated */
    } else { 
      // Receive the row
      t1 = timer();
      //MPI_Recv(&buffer[pivot], N - pivot, MPI_DOUBLE, cur_rank, 1, MPI_COMM_WORLD,&status);
      MPI_Bcast(&buffer[pivot], N - pivot, MPI_DOUBLE, cur_rank, MPI_COMM_WORLD);
      //MPI_Barrier(MPI_COMM_WORLD);
      param->t_comm += timer() - t1;
      // If there are still rows to recieve 
      if (row_index < blocking_factor) {
        // Eliminate all owned rows that are relevent
        for (row = row_index; row < blocking_factor; row++) {
          factor = R[row][pivot] / buffer[pivot];
          for (col = pivot; col < N; col++) { 
            R[row][col] = R[row][col] - factor * buffer[col];
          }
        }
      }
    }
  }
}

/**
 *	Reduce the rows back into a matrix
 */
void bcast_cycl_reduce_rows(struct GEmpi *param)
{
  int N = param->N;
  int size = param->size;  
  int rank = param->rank; 
  double **R = param->R;
  int blocking_factor = param->blocking_factor;

	MPI_Status status;
	int row, buf;
  double t1;
  double buffer[N];

  if (rank == 0) {
    //printf("\n\nPrinting output matrix:\n");

    int cur_rank;
    int r = -1;
    for (row = 0; row < N; row++) {
      cur_rank = row % size; 
      if (cur_rank == 0) {
        r++;
        //print_row(R[r],N);
      } else {
        t1 = timer();
        MPI_Send(&buf, 1, MPI_INT, cur_rank, 2, MPI_COMM_WORLD);
        MPI_Recv(&buffer, N, MPI_DOUBLE, cur_rank, 2, MPI_COMM_WORLD, &status);
        //print_row(buffer,N);
        param->t_comm += timer() - t1;
      }
    }
  } else {
    for (row = 0; row < blocking_factor; row++) {
      t1 = timer();
      MPI_Recv(&buf, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
      MPI_Send(R[row], N, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
      param->t_comm += timer() - t1;
    }
  }
}

/**
 *  Print the results to a file 
 */
void bcast_cycl_report_result(struct GEmpi *param) 
{
  //printf("rank = %i | blocking_factor = %i | t_exec = %f | t_proc = %f | t_comm = %f \n", param->rank, param->blocking_factor, param->t_exec, param->t_proc, param->t_comm); 
  
  int N = param->N;
  int size = param->size;  
  int rank = param->rank; 
  double **R = param->R;
  int blocking_factor = param->blocking_factor;

  MPI_Status status;
  int buf;

  char file_name[50];
  sprintf(file_name,"./results/bcast-cycl-%i-%i.dat",param->N,param->size);

  if (rank == 0) {
    write_to_file(file_name, param);
    int cur_rank, buf;
    for (cur_rank = 1; cur_rank < size; cur_rank++) {
      MPI_Send(&buf, 1, MPI_INT, cur_rank, 1, MPI_COMM_WORLD);
      MPI_Recv(&buf, 1, MPI_INT, cur_rank, 1, MPI_COMM_WORLD, &status);
    }
  } else {
    MPI_Recv(&buf, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    write_to_file(file_name, param);
    MPI_Send(&buf, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
  }

}

