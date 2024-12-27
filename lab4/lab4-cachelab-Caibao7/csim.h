#ifndef CSIM_H
#define CSIM_H

#include <limits.h>

// 全局变量声明
extern int h;
extern int v;
extern int s;
extern int E;
extern int b;
extern int S;

extern int hit_count;
extern int miss_count;
extern int eviction_count;

extern char t[1000];

// 替换策略：0 = LRU, 1 = 2Q
extern int policy;

// 缓存行结构体
typedef struct {
    int valid_bits;
    int tag;
    int stamp; // 用于LRU
} cache_line, *cache_asso, **cache;

// 2Q 队列节点
typedef struct q_node {
    int tag;
    struct q_node* next;
} q_node;

// 2Q 队列结构
typedef struct {
    q_node* head;
    q_node* tail;
    int size;
} queue;

// 每个set的2Q队列
typedef struct {
    queue A1_in;
    queue Am;
} two_q_set;

// 缓存模拟器的结构体
typedef struct {
    cache_line** lines;       // LRU 时使用
    two_q_set* two_q_sets;    // 2Q 时使用
} cache_struct;

extern cache_struct _cache_;

// 函数声明
void printUsage();
void init_cache();
void update(unsigned long address);
void parse_trace();

// 2Q 队列操作函数
void init_queue(queue* q);
int Enqueue(queue* q, int tag);
int Dequeue(queue* q, int* tag);
int is_in_queue(queue* q, int tag);

#endif // CSIM_H
