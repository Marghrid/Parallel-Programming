#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include <pthread.h>

#include "mandelbrot_set.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
	int *y_pt;
	int x_res;
	int y_res;
	int max_iter;
	double view_x0;
	double view_y1;
	double x_stepsize;
	double y_stepsize;
	int palette_shift;
	void* image;
} thread_args;


void* kernel (void* args) {
	double y;
	double x;

	complex double Z;
	complex double C;

	int k;

	thread_args *arg = (thread_args*) args;

	unsigned char (*image)[arg->x_res][3];
	image = arg->image;


	int* y_pt = arg->y_pt;
	int i;
	int terminate;

	while(1) {
    	pthread_mutex_lock(&mutex);

		i = (*(y_pt))++;

    	pthread_mutex_unlock(&mutex);

    	if (i >= arg->y_res)
    		break;

		for (int j = 0; j < arg->x_res; j++)
		{
			y = arg->view_y1 - i * arg->y_stepsize;
			x = arg->view_x0 + j * arg->x_stepsize;

			Z = 0 + 0 * I;
			C = x + y * I;

			k = 0;

			do
			{
				Z = Z * Z + C;
				k++;
			} while (cabs(Z) < 2 && k < arg->max_iter);

			if (k == arg->max_iter)
			{
				memcpy(image[i][j], "\0\0\0", 3);
			}
			else
			{
				int index = (k + arg->palette_shift)
				% (sizeof(colors) / sizeof(colors[0]));
				memcpy(image[i][j], colors[index], 3);
			}
		}
	}
	return NULL;
}

void mandelbrot_draw(int x_resolution, int y_resolution, int max_iter,
	double view_x0, double view_x1, double view_y0, double view_y1,
	double x_stepsize, double y_stepsize,
	int palette_shift, unsigned char (*image)[x_resolution][3],
	int num_threads) {

	pthread_t *threads;

	int *curr_y;
	curr_y = (int*)calloc(1, sizeof(int));

    /* IDEA: Organize all lines in a queue, kept by means of the synchronous
        variable curr_y. Once done, each thread checks the value of this
        variable and computes the next line, if there are more to compute.*/

    // To avoid unnecessary threads:
    /*
    if (y_resolution < num_threads) {
        num_threads = y_resolution;
    }
    */

	threads = (pthread_t*) calloc (num_threads, sizeof(pthread_t));
	thread_args* arg = (thread_args*) calloc (1, sizeof (thread_args));

	arg->y_pt = curr_y;
	arg->x_res = x_resolution;
	arg->y_res = y_resolution;
	arg->max_iter = max_iter;
	arg->view_x0 = view_x0;
	arg->view_y1 = view_y1;
	arg->x_stepsize = x_stepsize;
	arg->y_stepsize = y_stepsize;
	arg->palette_shift = palette_shift;
	arg->image = image;
	
	for (int i = 0; i < num_threads; ++i) {
		pthread_create(&threads[i], NULL, kernel, arg);
	}

	for (int i = 0; i < num_threads; ++i) {
		pthread_join(threads[i], NULL);
	}

	free(curr_y);
	free(threads);
	free (arg);
}
