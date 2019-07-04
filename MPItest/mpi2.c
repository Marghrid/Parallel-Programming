#include <mpi.h>
#include <stdio.h>

int main (int argc, char* argv[])  {
	int rank, size, tmp;

	MPI_Init(&argc, &argv); /* starts MPI */
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); /* process id */
	MPI_Comm_size(MPI_COMM_WORLD, &size); /* number processes */

/*
int MPI_Sendrecv(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                int dest, int sendtag,
                void *recvbuf, int recvcount, MPI_Datatype recvtype,
                int source, int recvtag,
                MPI_Comm comm, MPI_Status *status)
*/

	MPI_Sendrecv(&rank, 1, MPI_INT,
		( rank+1 ) % size, 0,
		&tmp, 1, MPI_INT, ( rank-1 ) % size, 0,
		MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	printf("I sent %d and received %d.\n", rank, tmp);

	MPI_Finalize();
	return 0;
}