#include <string.h>
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include "search.h"
#include "helper.h"

void search_text (char* text, int num_lines, int line_length, char* search_string, int *occurences)
{
	/*
	Counts occurences of substring "search_string" in "text". "text" contains multiple lines and each line
	has been placed at text + line_length * num_lines since line length in the original text file can vary.
	"line_length" includes space for '\0'.

	Writes result at location pointed to by "occurences".


	*************************** PARALLEL VERSION **************************

	NOTE: For the parallel version, distribute the lines to each processor. You should only write
	to "occurences" from the root process and only access the text pointer from the root (all other processes
	call this function with text = NULL) 
	*/

	int rank, size;
	
	*occurences = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); /* process id */
	MPI_Comm_size(MPI_COMM_WORLD, &size); /* number processes */
	
	int lines_per_process = num_lines/(size-1); // integer division. not counting root
	char recv[lines_per_process*line_length];

	if (rank == 0) {

		for (int i = 0; i < size-1; ++i) {
			// root process sends chunk of text to all others
			MPI_Send(text + i * lines_per_process * line_length, lines_per_process*line_length, MPI_CHAR, i+1, i+1, MPI_COMM_WORLD);
			//printf("%d send \"%s\" to %d.\n", rank, text + i * line_length, i+1);
		}
		// root process counts remainder words
		for (int i = lines_per_process*(size-1); i < num_lines; ++i) {
			*occurences += count_occurences(text + i * line_length, search_string);
		}


		for (int i = 0; i < size-1; ++i) {
			// root process sends chunk of text to all others
			int proc_count;
			MPI_Recv(&proc_count, 1, MPI_INT, i+1, size+i+1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			*occurences += proc_count;
		}
	}
	else {
		MPI_Recv(recv, lines_per_process*line_length, MPI_CHAR, 0, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		//printf("%d recv \"%s\" from %d.\n", rank, recv, 0);
		int count = 0;
		for (int i = 0; i < lines_per_process; i++) {
			count += count_occurences(recv + i * line_length, search_string);
		}
		MPI_Send(&count, 1, MPI_INT, 0, size+rank, MPI_COMM_WORLD);
		//printf("%d send \"%d\" to %d.\n", rank, count, 0);
	}
}
