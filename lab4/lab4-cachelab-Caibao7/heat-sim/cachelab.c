/*
 * cachelab.c - Cache Lab helper functions
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cachelab.h"
#include <time.h>


heat_func_t heat_func_list[MAX_FUNCS];

int heat_func_counter = 0;

/* 
 * printSummary - Summarize the cache simulation statistics. Student cache simulators
 *                must call this function in order to be properly autograded. 
 */
void printSummary(int hits, int misses, int evictions)
{
    printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);
    FILE* output_fp = fopen(".csim_results", "w");
    assert(output_fp);
    fprintf(output_fp, "%d %d %d\n", hits, misses, evictions);
    fclose(output_fp);
}

/* 
 * initMatrix - Initialize the given matrix 
 */
void initMatrix(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;
    srand(time(NULL));
    for (i = 0; i < N; i++){
        for (j = 0; j < M; j++){
            // A[i][j] = i+j;  /* The matrix created this way is symmetric */
            A[i][j]=rand();
            B[j][i]=rand();
        }
    }
}

void randMatrix(int M, int N, int A[N][M]) {
    int i, j;
    srand(time(NULL));
    for (i = 0; i < N; i++){
        for (j = 0; j < M; j++){
            // A[i][j] = i+j;  /* The matrix created this way is symmetric */
            A[i][j]=rand();
        }
    }
}

/* 
 * correctTrans - baseline transpose function used to evaluate correctness 
 */
void correctTrans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;
    for (i = 0; i < N; i++){
        for (j = 0; j < M; j++){
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    
}


void registerHeatFunction(void (*heat)(int T, int N, int A[T][N]), 
                           char* desc)
{
    heat_func_list[heat_func_counter].func_ptr = heat;
    heat_func_list[heat_func_counter].description = desc;
    heat_func_list[heat_func_counter].correct = 0;
    heat_func_list[heat_func_counter].num_hits = 0;
    heat_func_list[heat_func_counter].num_misses = 0;
    heat_func_list[heat_func_counter].num_evictions =0;
    heat_func_counter++;
}
