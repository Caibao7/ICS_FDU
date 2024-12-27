#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include "cachelab.h"
#include <string.h>

extern heat_func_t heat_func_list[MAX_FUNCS];
extern int heat_func_counter;
extern void registerHeatFunctions();

volatile char MARKER_START, MARKER_END;

static int A[1024][1024];
static int B[1024][1024];
static int T;
static int N;

static inline int kernel(int *w)
{
    return w[0] + (w[-1] - 2 * w[0] + w[1]);
}

void correctHeat(int T, int N, int B[T][N])
{
    for (int t = 0; t < T; t++)
        for (int x = 1; x < N; x++)
            B[t + 1][x] = kernel(&B[t][x]);
}

int validate(int fn, int T, int N, int A[T][N], int B[T][N])
{
    for (int i = 0; i < N; i++)
        B[0][i] = A[0][i];
    correctHeat(T, N, B);
    for (int t = 0; t < T; t++)
    {
        for (int i = 0; i < N; i++)
        {
            if (A[t][i] != B[t][i])
            {
                printf("Validation failed on function %d! Expected %d but got %d at A[%d][%d]\n", fn, B[t][i], A[t][i], t, i);
                return 0;
            }
        }
    }
    return 1;
}

void randMatrixForHeat(int T, int N, int A[T][N]){
    memset(A, 0, sizeof(int[N])*T);
    for(int x=0;x<N;x++){
        A[0][x] = rand()%100;
    }
}

int main(int argc, char *argv[])
{
    int i;

    char c;
    int selectedFunc = -1;

    while ((c = getopt(argc, argv, "T:N:F:")) != -1)
    {
        switch (c)
        {
        case 'T':
            T = atoi(optarg);
            break;
        case 'N':
            N = atoi(optarg);
            break;
        case 'F':
            selectedFunc = atoi(optarg);
            break;
        case '?':
        default:
            printf("./tracegen failed to parse its options.\n");
            exit(1);
        }
    }

    /*  Register transpose functions */
    registerHeatFunctions();

    /* Fill A with data */
    randMatrixForHeat(T, N, A);
    randMatrixForHeat(T, N, B);

    /* Record marker addresses */
    FILE *marker_fp = fopen(".marker", "w");
    assert(marker_fp);
    fprintf(marker_fp, "%llx %llx %llx %llx",
            (unsigned long long int)&MARKER_START,
            (unsigned long long int)&MARKER_END,
            (unsigned long long int)A,
            (unsigned long long int)B);
    fclose(marker_fp);

    if (-1 == selectedFunc)
    {
        /* Invoke registered transpose functions */
        for (i = 0; i < heat_func_counter; i++)
        {
            MARKER_START = 33;
            (*heat_func_list[i].func_ptr)(T, N, A);
            MARKER_END = 34;
            if (!validate(i, T, N, A, B))
            {
                return i + 1;
            }
        }
    }
    else
    {
        MARKER_START = 33;
        (*heat_func_list[selectedFunc].func_ptr)(T, N, A);
        MARKER_END = 34;
        if (!validate(selectedFunc, T, N, A, B))
        {
            return selectedFunc + 1;
        }
    }
    return 0;
}
