
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <omp.h>



#define DGEMM_RESTRICT __restrict__

// ------------------------------------------------------- //
// Function: get_seconds
//
// Vendor may modify this call to provide higher resolution
// timing if required
// ------------------------------------------------------- //
double get_seconds() {
    struct timeval now;
    gettimeofday(&now, NULL);

    const double seconds = (double) now.tv_sec;
    const double usec    = (double) now.tv_usec;

    return seconds + (usec * 1.0e-6);
}

// ------------------------------------------------------- //
// Function: main
//
// Modify only in permitted regions (see comments in the
// function)
// ------------------------------------------------------- //
int main(int argc, char* argv[])
{
    // ------------------------------------------------------- //
    // DO NOT CHANGE CODE BELOW
    // ------------------------------------------------------- //

    int N = 256;
    bool enable_error_check = false;
    int number_of_threads = 1;

    int repeats = 5;

    double alpha = 1.0;
    double beta  = 1.0;

    if(argc > 1) {
        number_of_threads = atoi(argv[1]);
        printf("Number of Threads: %d\n", number_of_threads);
        // Set Number of Threads to Run

        omp_set_dynamic(0);     // Explicitly disable dynamic teams
        omp_set_num_threads(number_of_threads);

        if(argc > 2) {
            N = atoi(argv[2]);
            printf("Matrix size input by command line: %d\n", N);

            if(argc > 3) {

                repeats = atoi(argv[3]);
                if(repeats < 5) {
                    fprintf(stderr, "Error: repeats must be at least 5, setting is: %d\n", repeats);
                    exit(-1);
                }

                if(argc > 4) {

                    printf("Repeat multiply %d times.\n", repeats);
                    if ((int) atoi(argv[4]) == 1) {
                        enable_error_check = true;
                    }

                    if(argc > 5) {
                        alpha = (double) atof(argv[5]);

                        if (argc > 6) {
                            beta = (double) atof(argv[6]);
                        }
                    }
                }
            }
        }
    }

    printf("Alpha =    %f\n", alpha);
    printf("Beta  =    %f\n", beta);

    if(N < 128) {
        printf("Error: N (%d) is less than 128, the matrix is too small.\n", N);
        exit(-1);
    }

    printf("Allocating Matrices...\n");

    double* DGEMM_RESTRICT matrixA = (double*) malloc(sizeof(double) * N * N);
    double* DGEMM_RESTRICT matrixB = (double*) malloc(sizeof(double) * N * N);
    double* DGEMM_RESTRICT matrixC = (double*) malloc(sizeof(double) * N * N);

    printf("Allocation complete, populating with values...\n");

    int i, j, k, r;
    int numChunks = N/128;
    int chunkIndex;

#pragma omp parallel for private (j)
    for(i = 0; i < N; i++) {
        for(j = 0; j < N; j++) {
            matrixA[i*N + j] = 2.0;
            matrixB[i*N + j] = 0.5;
            matrixC[i*N + j] = 1.0;
        }
    }

    printf("Performing multiplication...\n");

    const double start = get_seconds();

    // ------------------------------------------------------- //
    // VENDOR NOTIFICATION: START MODIFIABLE REGION
    //
    // Vendor is able to change the lines below to call optimized
    // DGEMM or other matrix multiplication routines. Do *NOT*
    // change any lines above this statement.
    // ------------------------------------------------------- //

    double sum = 0;

    // Repeat multiple times
    for(r = 0; r < repeats; r++) {
#pragma omp parallel for private(sum, k, j)
	for (chunkIndex = 1; chunkIndex <= numChunks; chunkIndex++) {
		for(i = 0; i < N; i++) {
		    for(j = 0; j < N; j++) {
		        sum = 0;

		        for(k = (chunkIndex-1)*128; k < chunkIndex*128; k++) {
		            sum += matrixA[i*N + k] * matrixB[k*N + j];
		        }

		        matrixC[i*N + j] = (alpha * sum) + (beta * matrixC[i*N + j]);
		    }
		}
	}
}

    // ------------------------------------------------------- //
    // VENDOR NOTIFICATION: END MODIFIABLE REGION
    // ------------------------------------------------------- //

    // ------------------------------------------------------- //
    // DO NOT CHANGE CODE BELOW
    // ------------------------------------------------------- //

    const double end = get_seconds();

    printf("Calculating matrix check...\n");

    double final_sum = 0;
    long long int count     = 0;

#pragma omp parallel for reduction(+:final_sum, count)
    for(i = 0; i < N; i++) {
        for(j = 0; j < N; j++) {
            final_sum += matrixC[i*N + j];
            count++;
        }
    }

    double N_dbl = (double) N;
    double matrix_memory = (3 * N_dbl * N_dbl) * ((double) sizeof(double));

    printf("\n");
    printf("===============================================================\n");

    const double count_dbl = (double) count;
    const double scaled_result = (final_sum / (count_dbl * repeats));

    printf("Final Sum is:         %f\n", scaled_result);

    const double check_sum = N_dbl + (1.0 / (double) (repeats));
    const double allowed_margin = 1.0e-8;

    if (enable_error_check){
        if( (check_sum >= (scaled_result - allowed_margin)) &&
                (check_sum <= (scaled_result + allowed_margin)) ) {
            printf(" -> Solution check PASSED successfully.\n");
        } else {
            printf(" -> Solution check FAILED.\n");
        }
    }

    printf("Memory for Matrices:  %f MB\n",
            (matrix_memory / (1024 * 1024)));

    const double time_taken = (end - start);

    printf("Multiply time:        %f seconds\n", time_taken);

    const double flops_computed = (N_dbl * N_dbl * N_dbl * 2.0 * (double)(repeats)) +
        (N_dbl * N_dbl * 2 * (double)(repeats));

    printf("FLOPs computed:       %f\n", flops_computed);
    printf("GFLOP/s rate:         %f GF/s\n", (flops_computed / time_taken) / 1000000000.0);

    printf("===============================================================\n");
    printf("\n");

    free(matrixA);
    free(matrixB);
    free(matrixC);

    return 0;
}
