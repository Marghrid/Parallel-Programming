#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include <pthread.h>

#include "mandelbrot_set.h"

typedef struct {
    int y_res;
    int x_res;
    int first_y;
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


    for (int i = arg->first_y; i < arg->y_res + arg->first_y; i++)
    {
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

    int curr_first_y = 0;

    // Distribute lines among threads. 
    // Each thread does all the pixels in its lines

    // To avoid unnecessary threads:
    /*
    if (y_resolution < num_threads) {
        num_threads = y_resolution;
    }
    */

    threads = (pthread_t*) malloc (num_threads *sizeof(pthread_t));
    thread_args* args = (thread_args*) malloc (num_threads*sizeof (thread_args));

    int lines_per_thread = y_resolution / num_threads; //integer division
     //remainder lines distributed among the threads
    int remainder = y_resolution - num_threads*lines_per_thread;

    for (int i = 0; i < num_threads; ++i) {
        if (remainder > 0) {
            args[i].y_res = lines_per_thread + 1;
            --remainder;
        } else {
            args[i].y_res = lines_per_thread;
        }

        args[i].x_res = x_resolution;
        args[i].first_y = curr_first_y;
        curr_first_y += args[i].y_res;
        args[i].max_iter = max_iter;
        args[i].view_x0 = view_x0;
        args[i].view_y1 = view_y1;
        args[i].x_stepsize = x_stepsize;
        args[i].y_stepsize = y_stepsize;
        args[i].palette_shift = palette_shift;
        args[i].image = image;

        pthread_create(&threads[i], NULL, kernel, args+i);
    }

    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    free (args);
}
