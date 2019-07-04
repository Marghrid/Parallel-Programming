#include <mpi.h>
#include <stdio.h>

int main (int argc, char* argv[]) {
int rank, size;

MPI_Init(&argc, &argv); /* starts MPI */
MPI_Comm_rank(MPI_COMM_WORLD, &rank); /* process id */
MPI_Comm_size(MPI_COMM_WORLD, &size); /* number processes */
printf( "Hello world from process %d of %d\n", rank, size );
MPI_Finalize();
return 0;
}