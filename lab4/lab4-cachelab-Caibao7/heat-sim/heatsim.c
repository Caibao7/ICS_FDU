#include <stdio.h>
#include <stdlib.h>
#include "cachelab.h"

static inline int kernel(int *w)
{
    // we set alpha to 1 and delta_t to 1.
    // equals to w[0] + alpha * (w[-1] - 2 * w[0] + w[1]);
    // example usage: A[t + 1][x] = kernel(&A[t][x]);
    return (w[-1] - w[0] + w[1]);
}

char heat_sim_desc[] = "Heat Simulate submission";
void heat_sim_example(int T, int N, int A[T][N])
{
    for (int t = 0; t < T; t++)
        for (int x = 1; x < N; x++)
            A[t + 1][x] = kernel(&A[t][x]);
}

void heat_sim(int T, int N, int A[T][N])
{   
    // int ALPHA = 1;
    // int DELTA_T = 1;
    // 分配缓冲
    int *current = malloc(N * sizeof(int));
    int *next = malloc(N * sizeof(int));

    // 初始化 current 为 A[0][x]
    for (int x = 0; x < N; x++) {
        current[x] = A[0][x];
    }

    for (int t = 0; t < T; t++) {
        // 计算下一时间的温度
        for (int x = 1; x < N; x++) {
            next[x] = current[x-1] - current[x] + current[x+1];
        }
        // 处理边界点，调试输出说x=0的点都是0，所以直接赋值为0
        next[0] = 0;
        // next[N-1] = A[t][N-1];

        // 将 next 的结果保存到 A[t+1][x]
        for (int x = 0; x < N; x++) {
            A[t+1][x] = next[x];
        }

        // 交换 current 和 next
        int *temp = current;
        current = next;
        next = temp;
    }

    // 释放内存
    free(current);
    free(next);
}


void registerHeatFunctions()
{
    // registerHeatFunction(heat_sim_example, heat_sim_desc);
    registerHeatFunction(heat_sim, heat_sim_desc);
}