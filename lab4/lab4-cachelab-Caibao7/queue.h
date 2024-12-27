#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


typedef struct queue_node {
    int tag;                 // 缓存行的标记
    struct queue_node *next; // 指向下一个节点
} QueueNode;

typedef struct queue {
    QueueNode *front; // 队首
    QueueNode *rear;  // 队尾
    int size;         // 当前队列大小
} Queue;

Queue *create_queue();
void enqueue(Queue *q, int tag);
int dequeue(Queue *q);
int find_and_remove(Queue *q, int tag);