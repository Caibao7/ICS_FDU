/*
 * test-trans.c - Checks the correctness and performance of all of the
 *     student's transpose functions and records the results for their
 *     official submitted version as well.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <sys/types.h>
#include "cachelab.h"
#include <sys/wait.h> // fir WEXITSTATUS
#include <limits.h>   // for INT_MAX

/* Maximum array dimension */
#define MAXN 1024
#define TEST_CNT 3

/* The description string for the transpose_submit() function that the
   student submits for credit */
#define SUBMIT_DESCRIPTION "Heat Simulate submission"

extern heat_func_t heat_func_list[MAX_FUNCS];
extern int heat_func_counter;
extern void registerHeatFunctions();

/* Globals set on the command line */
static int T = 100;
static int N = 512;

/* The correctness and performance for the submitted heat function */
struct results
{
    int funcid;
    int correct;
    int misses;
};
static struct results results = {-1, 0, INT_MAX};
// static double missrate_threshold[3] = {0.12, 0.10, 0.078};
static int miss_threshold[3] = {22500, 18000, 15720};
/*
 * eval_perf - Evaluate the performance of the registered transpose functions
 */
int eval_perf(unsigned int s, unsigned int E, unsigned int b)
{
    int i, flag;
    unsigned int len, hits, misses, evictions;
    unsigned long long int marker_start, marker_end, addr;
    unsigned long long int Astart, Aend, Bstart;
    char buf[1000], cmd[255];
    char filename[128];
    /* Open the complete trace file */
    FILE *full_trace_fp;
    FILE *part_trace_fp;

    /* Evaluate the performance of each registered transpose function */

    for (i = 0; i < heat_func_counter; i++)
    {
        printf("begin heatfunc %i", i);
        if (strcmp(heat_func_list[i].description, SUBMIT_DESCRIPTION) == 0)
            results.funcid = i; /* remember which function is the submission */

        printf("\nFunction %d (%d total)\nStep 1: Validating and generating memory traces\n", i, heat_func_counter);
        /* Use valgrind to generate the trace */

        sprintf(cmd, "valgrind --tool=lackey --trace-mem=yes --log-fd=1 -v ./heatgen -T %d -N %d -F %d  > trace.tmp", T, N, i);
        flag = WEXITSTATUS(system(cmd));
        if (0 != flag)
        {
            printf("Validation error at function %d! Run ./heatgen -T %d -N %d -F %d for details.\nSkipping performance evaluation for this function.\n", flag - 1, T, N, i);
            continue;
        }

        /* Get the start and end marker addresses */
        FILE *marker_fp = fopen(".marker", "r");
        assert(marker_fp);

        fscanf(marker_fp, "%llx %llx %llx %llx", &marker_start, &marker_end, &Astart, &Bstart);
        fclose(marker_fp);

        heat_func_list[i].correct = 1;

        /* Save the correctness of the transpose submission */
        if (results.funcid == i)
        {
            results.correct = 1;
        }

        full_trace_fp = fopen("trace.tmp", "r");
        assert(full_trace_fp);

        /* Filtered trace for each transpose function goes in a separate file */
        sprintf(filename, "trace.f%d", i);
        part_trace_fp = fopen(filename, "w");
        assert(part_trace_fp);

        /* Locate trace corresponding to the trans function */
        flag = 0;
        Aend = Astart + 1024 * 1024 * sizeof(int);
        unsigned long long int Adelta = 0;
        while (fgets(buf, 1000, full_trace_fp) != NULL)
        {
            /* We are only interested in memory access instructions */
            if (buf[0] == ' ' && buf[2] == ' ' &&
                (buf[1] == 'S' || buf[1] == 'M' || buf[1] == 'L'))
            {
                sscanf(buf + 3, "%llx,%u", &addr, &len);

                /* If start marker found, set flag */
                if (addr == marker_start)
                {
                    flag = 1;
                }

                /* Valgrind creates many spurious accesses to the
                   stack that have nothing to do with the students
                   code. At the moment, we are ignoring all stack
                   accesses by using the simple filter of recording
                   accesses to only the low 32-bit portion of the
                   address space. At some point it would be nice to
                   try to do more informed filtering so that would
                   eliminate the valgrind stack references while
                   include the student stack references. */
                if (flag && addr < 0xffffffff)
                {
                    // align the start address to be the multiply of 48
                    if (addr >= Astart && addr < Aend)
                    {
                        addr = addr + Adelta;
                        sprintf(buf + 3, "%llx,%u\n", addr, len);
                        fputs(buf, part_trace_fp);
                    }
                }

                /* if end marker found, close trace file */
                if (addr == marker_end)
                {
                    flag = 0;
                    fclose(part_trace_fp);
                    break;
                }
            }
        }
        fclose(full_trace_fp);

        /* Run the reference simulator */
        printf("Step 2: Evaluating performance (s=%d, E=%d, b=%d)\n", s, E, b);
        char cmd[255];
        sprintf(cmd, "./csim-ref -s %u -E %u -b %u -t trace.f%d -q LRU > /dev/null",
                s, E, b, i);
        system(cmd);

        /* Collect results from the reference simulator */
        FILE *in_fp = fopen(".csim_results", "r");
        assert(in_fp);
        fscanf(in_fp, "%u %u %u", &hits, &misses, &evictions);
        fclose(in_fp);
        heat_func_list[i].num_hits = hits;
        heat_func_list[i].num_misses = misses;
        heat_func_list[i].num_evictions = evictions;
        printf("func %u (%s): hits:%u, misses:%u, evictions:%u\n",
               i, heat_func_list[i].description, hits, misses, evictions);

        /* If it is transpose_submit(), record number of misses */
        if (results.funcid == i)
        {
            results.misses = misses;
        }
    }
    return results.misses;
}

/*
 * usage - Print usage info
 */
void usage(char *argv[])
{
    printf("Usage: %s [-h] -T <t> -N <x>\n", argv[0]);
    printf("Options:\n");
    printf("  -h          Print this help message.\n");
    printf("  -T <t>   Number of time steps (max %d)\n", MAXN);
    printf("  -N <x>   Number of points (max %d)\n", MAXN);
    printf("Example: %s -T 8 -N 8\n", argv[0]);
}

/*
 * sigsegv_handler - SIGSEGV handler
 */
void sigsegv_handler(int signum)
{
    printf("Error: Segmentation Fault.\n");
    printf("TEST_TRANS_RESULTS=0:0\n");
    fflush(stdout);
    exit(1);
}

/*
 * sigalrm_handler - SIGALRM handler
 */
void sigalrm_handler(int signum)
{
    printf("Error: Program timed out.\n");
    printf("TEST_TRANS_RESULTS=0:0\n");
    fflush(stdout);
    exit(1);
}

/*
 * main - Main routine
 */
int main(int argc, char *argv[])
{
    char c;

    while ((c = getopt(argc, argv, "T:N:h")) != -1)
    {
        switch (c)
        {
        case 'T':
            T = atoi(optarg);
            break;
        case 'N':
            N = atoi(optarg);
            break;
        case 'h':
            usage(argv);
            exit(0);
        default:
            usage(argv);
            exit(1);
        }
    }

    if (T == 0 || N == 0)
    {
        printf("Error: Missing required argument\n");
        usage(argv);
        exit(1);
    }

    if (T > MAXN || N > MAXN)
    {
        printf("Error: M or N exceeds %d\n", MAXN);
        usage(argv);
        exit(1);
    }

    /* Install SIGSEGV and SIGALRM handlers */
    if (signal(SIGSEGV, sigsegv_handler) == SIG_ERR)
    {
        fprintf(stderr, "Unable to install SIGALRM handler\n");
        exit(1);
    }

    if (signal(SIGALRM, sigalrm_handler) == SIG_ERR)
    {
        fprintf(stderr, "Unable to install SIGALRM handler\n");
        exit(1);
    }

    /* Check the performance of the student's transpose function */
    registerHeatFunctions();
    int s[TEST_CNT] = {1, 1, 1};
    int E[TEST_CNT] = {16, 32, 64};
    int b[TEST_CNT] = {16, 16, 16};
    int score = 0;
    for (int i = 0; i < TEST_CNT; i++)
    {
        /* Time out and give up after a while */
        alarm(120);
        int miss = eval_perf(s[i], E[i], b[i]);
        if (miss <= miss_threshold[i])
            score += 3;
    }

    /* Emit the results for this particular test */
    if (results.funcid == -1)
    {
        printf("\nError: We could not find your transpose_submit() function\n");
        printf("Error: Please ensure that description field is exactly \"%s\"\n",
               SUBMIT_DESCRIPTION);
        printf("\nTEST_TRANS_RESULTS=0:0\n");
    }
    else
    {
        printf("\nTEST_HEAT_SCORES=%d\n", score);
    }
    return 0;
}
