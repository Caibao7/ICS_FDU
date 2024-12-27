/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */

char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, k, s;
    int a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11;
    
    if (M == 48 && N == 48) {
        for (i = 0; i < N; i += 12) {
            for (j = 0; j < N; j += 12) {
                // copy
                for (k = i, s = j; k < i + 12; k++, s++) {
                    a0 = A[k][j];
                    a1 = A[k][j + 1];
                    a2 = A[k][j + 2];
                    a3 = A[k][j + 3];
                    a4 = A[k][j + 4];
                    a5 = A[k][j + 5];
                    a6 = A[k][j + 6];
                    a7 = A[k][j + 7];
                    a8 = A[k][j + 8];
                    a9 = A[k][j + 9];
                    a10 = A[k][j + 10];
                    a11 = A[k][j + 11];
                    B[s][i] = a0;
                    B[s][i + 1] = a1;
                    B[s][i + 2] = a2;
                    B[s][i + 3] = a3;
                    B[s][i + 4] = a4;
                    B[s][i + 5] = a5;
                    B[s][i + 6] = a6;
                    B[s][i + 7] = a7;
                    B[s][i + 8] = a8;
                    B[s][i + 9] = a9;
                    B[s][i + 10] = a10;
                    B[s][i + 11] = a11;
                }
                // transpose
                for (k = 0; k < 12; k++) {
                    for (s = k + 1; s < 12; s++) {
                        a0 = B[k + j][s + i];
                        B[k + j][s + i] = B[s + j][k + i];
                        B[s + j][k + i] = a0;
                    }
                }
            }
        }
    }
    else if (M == 96 && N == 96) {
        int i, j, ii, jj;
        int a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11;

        for (jj = 0; jj < N; jj += 12){
            // 先处理对角块
            if (jj == 0) ii = 12; else ii = 0;

            for (i = jj; i < jj + 6; ++i){
                a0 = A[i+6][jj+0];
                a1 = A[i+6][jj+1];
                a2 = A[i+6][jj+2];
                a3 = A[i+6][jj+3];
                a4 = A[i+6][jj+4];
                a5 = A[i+6][jj+5];
                a6 = A[i+6][jj+6];
                a7 = A[i+6][jj+7];
                a8 = A[i+6][jj+8];
                a9 = A[i+6][jj+9];
                a10 = A[i+6][jj+10];
                a11 = A[i+6][jj+11];

                B[i][ii+0] = a0;
                B[i][ii+1] = a1;
                B[i][ii+2] = a2;
                B[i][ii+3] = a3;
                B[i][ii+4] = a4;
                B[i][ii+5] = a5;
                B[i][ii+6] = a6;
                B[i][ii+7] = a7;
                B[i][ii+8] = a8;
                B[i][ii+9] = a9;
                B[i][ii+10] = a10;
                B[i][ii+11] = a11;
            }

            for (i = 0; i < 6; ++ i){
                for (j = i + 1; j < 6; ++j){
                    a0 = B[jj+i][ii+j];
                    B[jj+i][ii+j] = B[jj+j][ii+i];
                    B[jj+j][ii+i] = a0;

                    a0 = B[jj+i][ii+j+6];
                    B[jj+i][ii+j+6] = B[jj+j][ii+i+6];
                    B[jj+j][ii+i+6] = a0;
                }
            }

            for (i = jj; i < jj + 6; ++i){
                a0 = A[i][jj+0];
                a1 = A[i][jj+1];
                a2 = A[i][jj+2];
                a3 = A[i][jj+3];
                a4 = A[i][jj+4];
                a5 = A[i][jj+5];
                a6 = A[i][jj+6];
                a7 = A[i][jj+7];
                a8 = A[i][jj+8];
                a9 = A[i][jj+9];
                a10 = A[i][jj+10];
                a11 = A[i][jj+11];

                B[i][jj+0] = a0;
                B[i][jj+1] = a1;
                B[i][jj+2] = a2;
                B[i][jj+3] = a3;
                B[i][jj+4] = a4;
                B[i][jj+5] = a5;
                B[i][jj+6] = a6;
                B[i][jj+7] = a7;
                B[i][jj+8] = a8;
                B[i][jj+9] = a9;
                B[i][jj+10] = a10;
                B[i][jj+11] = a11;
            }

            for (i = jj; i < jj + 6; ++i){
                for (j = i + 1; j < jj + 6; ++j){
                    a0 = B[i][j];
                    B[i][j] = B[j][i];
                    B[j][i] = a0;

                    a0 = B[i][j+6];
                    B[i][j+6] = B[j][i+6];
                    B[j][i+6] = a0;
                }
            }
            
            for (i = 0; i < 6; ++ i){
                a0 = B[jj+i][jj+6];
                a1 = B[jj+i][jj+7];
                a2 = B[jj+i][jj+8];
                a3 = B[jj+i][jj+9];
                a4 = B[jj+i][jj+10];
                a5 = B[jj+i][jj+11];

                B[jj+i][jj+6] = B[jj+i][ii+0];
                B[jj+i][jj+7] = B[jj+i][ii+1];
                B[jj+i][jj+8] = B[jj+i][ii+2];
                B[jj+i][jj+9] = B[jj+i][ii+3];
                B[jj+i][jj+10] = B[jj+i][ii+4];
                B[jj+i][jj+11] = B[jj+i][ii+5];

                B[jj+i][ii+0] = a0;
                B[jj+i][ii+1] = a1;
                B[jj+i][ii+2] = a2;
                B[jj+i][ii+3] = a3;
                B[jj+i][ii+4] = a4;
                B[jj+i][ii+5] = a5;

            }

            for (i = 0; i < 6; ++ i){
                B[jj+i+6][jj+0] = B[jj+i][ii+0];
                B[jj+i+6][jj+1] = B[jj+i][ii+1];
                B[jj+i+6][jj+2] = B[jj+i][ii+2];
                B[jj+i+6][jj+3] = B[jj+i][ii+3];
                B[jj+i+6][jj+4] = B[jj+i][ii+4];
                B[jj+i+6][jj+5] = B[jj+i][ii+5];
                B[jj+i+6][jj+6] = B[jj+i][ii+6];
                B[jj+i+6][jj+7] = B[jj+i][ii+7];
                B[jj+i+6][jj+8] = B[jj+i][ii+8];
                B[jj+i+6][jj+9] = B[jj+i][ii+9];
                B[jj+i+6][jj+10] = B[jj+i][ii+10];
                B[jj+i+6][jj+11] = B[jj+i][ii+11];
            }

            // 处理非对角块
            for (ii = 0; ii < M; ii += 12){
                if (ii == jj){
                    // 跳过对角块
                    continue;
                }else{
                    for (i = ii; i < ii + 6; ++i){
                        a0 = A[i][jj+0];
                        a1 = A[i][jj+1];
                        a2 = A[i][jj+2];
                        a3 = A[i][jj+3];
                        a4 = A[i][jj+4];
                        a5 = A[i][jj+5];
                        a6 = A[i][jj+6];
                        a7 = A[i][jj+7];
                        a8 = A[i][jj+8];
                        a9 = A[i][jj+9];
                        a10 = A[i][jj+10];
                        a11 = A[i][jj+11];

                        B[jj+0][i] = a0;
                        B[jj+1][i] = a1;
                        B[jj+2][i] = a2;
                        B[jj+3][i] = a3;
                        B[jj+4][i] = a4;
                        B[jj+5][i] = a5;
                        B[jj+0][i+6] = a6;
                        B[jj+1][i+6] = a7;
                        B[jj+2][i+6] = a8;
                        B[jj+3][i+6] = a9;
                        B[jj+4][i+6] = a10;
                        B[jj+5][i+6] = a11;
                    }

                    for (j = jj; j < jj + 6; ++j){
                        a0 = A[ii+6][j];
                        a1 = A[ii+7][j];
                        a2 = A[ii+8][j];
                        a3 = A[ii+9][j];
                        a4 = A[ii+10][j];
                        a5 = A[ii+11][j];

                        a6 = B[j][ii+6];
                        a7 = B[j][ii+7];
                        a8 = B[j][ii+8];
                        a9 = B[j][ii+9];
                        a10 = B[j][ii+10];
                        a11 = B[j][ii+11];

                        B[j][ii+6] = a0;
                        B[j][ii+7] = a1;
                        B[j][ii+8] = a2;
                        B[j][ii+9] = a3;
                        B[j][ii+10] = a4;
                        B[j][ii+11] = a5;

                        B[j+6][ii+0] = a6;
                        B[j+6][ii+1] = a7;
                        B[j+6][ii+2] = a8;
                        B[j+6][ii+3] = a9;
                        B[j+6][ii+4] = a10;
                        B[j+6][ii+5] = a11;
                    }

                    for (i = ii + 6; i < ii + 12; ++i){
                        a0 = A[i][jj+6];
                        a1 = A[i][jj+7];
                        a2 = A[i][jj+8];
                        a3 = A[i][jj+9];
                        a4 = A[i][jj+10];
                        a5 = A[i][jj+11];

                        B[jj+6][i] = a0;
                        B[jj+7][i] = a1;
                        B[jj+8][i] = a2;
                        B[jj+9][i] = a3;
                        B[jj+10][i] = a4;
                        B[jj+11][i] = a5;
                    }
                }
            }
        }
    }
    else {
        // 处理其他矩阵大小（如果有的话），可以使用简单的转置方法
        for (i = 0; i < N; i++) {
            for (j = 0; j < M; j++) {
                B[j][i] = A[i][j];
            }
        }
    }
}



/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    // registerTransFunction(trans, trans_desc);
}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; ++j)
        {
            if (A[i][j] != B[j][i])
            {
                return 0;
            }
        }
    }
    return 1;
}
