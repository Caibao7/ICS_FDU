#include "cachelab.h"
#include "csim.h"  // 包含头文件
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <getopt.h>
#include <string.h>

// 全局变量定义
int h = 0;
int v = 0;
int s = 0, E = 0, b = 0, S = 0;

int hit_count = 0, miss_count = 0, eviction_count = 0;

char t[1000];

int policy = 0; // 0: LRU, 1: 2Q

cache_struct _cache_ = {0};  // 初始化为空

// 初始化队列
void init_queue(queue* q) {
    q->head = q->tail = NULL;
    q->size = 0;
}

// 入队，返回 1 表示成功，0 表示失败
int Enqueue(queue* q, int tag) {
    q_node* new_node = (q_node*)malloc(sizeof(q_node));
    if (!new_node) return 0;
    new_node->tag = tag;
    new_node->next = NULL;
    if (q->tail) {
        q->tail->next = new_node;
    } else {
        q->head = new_node;
    }
    q->tail = new_node;
    q->size++;
    return 1;
}

// 出队，返回 1 表示成功，0 表示队列为空
int Dequeue(queue* q, int* tag) {
    if (!q->head) return 0;
    q_node* temp = q->head;
    *tag = temp->tag;
    q->head = temp->next;
    if (!q->head) q->tail = NULL;
    free(temp);
    q->size--;
    return 1;
}

// 检查 tag 是否在队列中，存在返回 1，否则返回 0
int is_in_queue(queue* q, int tag) {
    q_node* current = q->head;
    while (current) {
        if (current->tag == tag) return 1;
        current = current->next;
    }
    return 0;
}


// 打印 helper 内容的函数，-h 命令使用，内容可自定义
void printUsage()
{
    printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file> -q <policy>\n"
           "Options:\n"
           "  -h         Print this help message.\n"
           "  -v         Optional verbose flag.\n"
           "  -s <num>   Number of set index bits.\n"
           "  -E <num>   Number of lines per set.\n"
           "  -b <num>   Number of block offset bits.\n"
           "  -t <file>  Trace file.\n"
           "  -q <policy> Replacement policy (LRU or 2Q).\n\n"
           "Examples:\n"
           "  linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace -q LRU\n"
           "  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace -q 2Q\n");
}

// 初始化cache的函数
void init_cache()
{
    _cache_.lines = (cache_line**)malloc(sizeof(cache_line*) * S);
    if (_cache_.lines == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for cache sets\n");
        exit(EXIT_FAILURE);
    }

    if(policy == 1){
        _cache_.two_q_sets = (two_q_set*)malloc(sizeof(two_q_set) * S);
        if (_cache_.two_q_sets == NULL) {
            fprintf(stderr, "Error: Unable to allocate memory for 2Q sets\n");
            exit(EXIT_FAILURE);
        }
        for(int i = 0; i < S; i++) {
            init_queue(&(_cache_.two_q_sets[i].A1_in));
            init_queue(&(_cache_.two_q_sets[i].Am));
        }
    }

    for(int i = 0; i < S; i++)
    {
        _cache_.lines[i] = (cache_line*)malloc(sizeof(cache_line) * E);
        if (_cache_.lines[i] == NULL) {
            fprintf(stderr, "Error: Unable to allocate memory for cache lines\n");
            exit(EXIT_FAILURE);
        }
        for(int j = 0; j < E; j++)
        { // 初始化每个组里的cache_line
            _cache_.lines[i][j].valid_bits = 0;
            _cache_.lines[i][j].tag = 0;
            _cache_.lines[i][j].stamp = 0;
        }
    }
}


// 更新cache的函数
void update(unsigned long address)
{
    // 计算组索引和标签
    int block_num = address / b;
    int setindex_add = block_num % S; // 组索引
    int tag_add = block_num / S; // 标签
    if(setindex_add > S - 1){
        tag_add += E*(setindex_add - S);
        setindex_add = setindex_add % S; // 如果组索引大于S-1，则标签需要增加E*(setindex_add - S)
    }

    // 调试
    // printf("setindex_add: %u, tag_add: %u ", setindex_add, tag_add);

    if(policy == 0){ // LRU
        int max_stamp = INT_MIN;
        int max_stamp_index = -1; 

        // 检查是否命中，逐行检查每个组里的行
        for(int i = 0; i < E; i++)
        {
            if(_cache_.lines[setindex_add][i].tag == tag_add && _cache_.lines[setindex_add][i].valid_bits)
            {
                _cache_.lines[setindex_add][i].stamp = 0; // 重置时间戳
                hit_count++;
                if(v) {
                    printf("hit ");
                }
                return;
            }
        }
        
        // 如果未命中，查找是否有空行
        for(int i = 0; i < E; i++)
        {
            if(_cache_.lines[setindex_add][i].valid_bits == 0)
            {
                // 调试
                /*printf("有空行 ");
                printf("before: %d ", _cache_[setindex_add][i].valid_bits);
                printf("%d ", _cache_[setindex_add][i].tag);
                printf("%d ", _cache_[setindex_add][i].stamp);*/
                _cache_.lines[setindex_add][i].valid_bits = 1;
                _cache_.lines[setindex_add][i].tag = tag_add;
                _cache_.lines[setindex_add][i].stamp = 0;
                miss_count++;
                if(v) {
                    printf("miss ");
                }
                // 调试
                /*printf("miss_count: %d ", miss_count);
                printf("after: %d ", _cache_[setindex_add][i].valid_bits);
                printf("%d ", _cache_[setindex_add][i].tag);
                printf("%d \n", _cache_[setindex_add][i].stamp);*/
                return ;
            }
        }

        // 没有空行又没有hit就是要替换了
        miss_count++; // 未命中次数加1，前面两步都会return，不用担心重复加
        eviction_count++; // 替换次数加1
        /*printf("没有hit也没有空行，要替换 ");*/ // 调试
        if(v) {
            printf("miss ");
            /*printf("%d ", miss_count);*/ // 调试
        }
        if(v) {
            printf("eviction ");
            /*printf("%d ", eviction_count);*/ // 调试
        }
        // 找到 LRU 行（最大的 stamp）
        for(int i = 0; i < E; ++i)
        {
            if(_cache_.lines[setindex_add][i].stamp > max_stamp)
            {
                max_stamp = _cache_.lines[setindex_add][i].stamp;
                max_stamp_index = i;
            }
            /*printf("max_stamp_index %d ", max_stamp_index); // 调试
            printf("max_stamp %d \n", max_stamp);*/ // 调试
        }
        _cache_.lines[setindex_add][max_stamp_index].tag = tag_add;
        _cache_.lines[setindex_add][max_stamp_index].stamp = 0;
        return ;
    }
        
    else if(policy == 1) { // 2Q
        two_q_set* current_set = &(_cache_.two_q_sets[setindex_add]);

        // 1. 检查 Am 队列是否命中
        if(is_in_queue(&current_set->Am, tag_add)) {
            hit_count++;
            if(v) printf("hit ");
            // 更新 Am 队列（将 tag 移到队尾）
            // 为简单起见，移除再入队
            // 移除 tag
            q_node* prev = NULL;
            q_node* current = current_set->Am.head;
            while(current) {
                if(current->tag == tag_add) break;
                prev = current;
                current = current->next;
            }
            if(current) {
                if(prev) {
                    prev->next = current->next;
                }
                else {
                    current_set->Am.head = current->next;
                }
                if(current_set->Am.tail == current) {
                    current_set->Am.tail = prev;
                }
                current_set->Am.size--;
                // 入队
                Enqueue(&current_set->Am, tag_add);
            }
        }
        else if(is_in_queue(&current_set->A1_in, tag_add)) {
            // 2. 检查 A1_in 队列是否存在
            miss_count++;
            if(v) printf("miss ");
            // 将 tag 从 A1_in 移除，并插入到 Am
            // 移除 tag
            q_node* prev = NULL;
            q_node* current = current_set->A1_in.head;
            while(current) {
                if(current->tag == tag_add) break;
                prev = current;
                current = current->next;
            }
            if(current) {
                if(prev) {
                    prev->next = current->next;
                }
                else {
                    current_set->A1_in.head = current->next;
                }
                if(current_set->A1_in.tail == current) {
                    current_set->A1_in.tail = prev;
                }
                current_set->A1_in.size--;
                free(current);
                // 插入到 Am
                Enqueue(&current_set->Am, tag_add);
                // 如果 Am 超过容量，驱逐最旧的
                if(current_set->Am.size > E) {
                    int evict_tag;
                    if(Dequeue(&current_set->Am, &evict_tag)) {
                        eviction_count++;
                        if(v) printf("eviction ");
                        // 将被驱逐的 tag 从 cache 中替换
                        // 查找 cache_line
                        int evict_line = -1;
                        for(int i = 0; i < E; i++) {
                            if(_cache_.lines[setindex_add][i].valid_bits && _cache_.lines[setindex_add][i].tag == evict_tag) {
                                evict_line = i;
                                break;
                            }
                        }
                        if(evict_line != -1) {
                            _cache_.lines[setindex_add][evict_line].tag = tag_add;
                            _cache_.lines[setindex_add][evict_line].stamp = 0;
                        }
                    }
                }
            }
        }
        else {
            // 3. 完全未命中，插入到 A1_in
            miss_count++;
            if(v) printf("miss ");
            // 如果 A1_in 未满，直接插入
            if(current_set->A1_in.size < E) {
                Enqueue(&current_set->A1_in, tag_add);
            }
            else {
                // A1_in 满，驱逐最旧的
                int evict_tag;
                if(Dequeue(&current_set->A1_in, &evict_tag)) {
                    // 插入新的 tag
                    Enqueue(&current_set->A1_in, tag_add);
                }
            }
        }
    }
}

void update_stamp()
{
	for(int i = 0; i < S; i++)
		for(int j = 0; j < E; j++)
			if(_cache_.lines[i][j].valid_bits == 1)
				_cache_.lines[i][j].stamp++;
}

// 解析trace文件并模拟缓存行为
void parse_trace()
{
	FILE* fp = fopen(t, "r"); // 读取文件名
	if(fp == NULL)
	{
		printf("open error");
		exit(-1);
	}
	
	char operation;         // 命令开头的 I L M S
	unsigned long address;   // 地址参数
	int size;               // 大小
	while(fscanf(fp, " %c %lx,%d\n", &operation, &address, &size) > 0)
	{
        if(v) {
            printf("%c %lx,%d ", operation, address, size);
        }

		switch(operation)
		{
			case 'I': // 忽略I操作
                if(v) {
                    printf("\n");
                }
                continue;	   
			case 'L':
				update(address);
				break;
			case 'M':
				update(address);  // miss的话还要进行一次storage
                update(address);
                break;
			case 'S':
				update(address);
                break;
		}
        if(v && operation != 'I') {
            printf("\n");
        }
		update_stamp();	//更新时间戳
	}
	
	fclose(fp);
	for(int i = 0; i < S; ++i)
    {
        if(policy == 0) { // LRU
            free(_cache_.lines[i]);
        }
        else if(policy == 1) { // 2Q
            // 释放 A1_in 队列
            q_node* current = _cache_.two_q_sets[i].A1_in.head;
            while(current != NULL) {
                q_node* temp = current;
                current = current->next;
                free(temp);
            }
            // 释放 Am 队列
            current = _cache_.two_q_sets[i].Am.head;
            while(current != NULL) {
                q_node* temp = current;
                current = current->next;
                free(temp);
            }
        }
    }
    free(_cache_.lines);
    if(policy == 1) {
        free(_cache_.two_q_sets);
    }
	
}

int main(int argc, char* argv[])
{
    // 初始化变量
    h = 0; 
    v = 0; 
    hit_count = miss_count = eviction_count = 0;
    int opt; // 接收getopt的返回值
            
    while(-1 != (opt = getopt(argc, argv, "hvs:E:b:t:q:")))
    {
        switch(opt)
        {
            case 'h':
                h = 1;
                printUsage();
                return 0; // 打印帮助后退出
            case 'v':
                v = 1;
                break;
            case 's':
                s = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b': {
                int block_size = atoi(optarg);
                if (block_size <= 0) {
                    fprintf(stderr, "Error: Block size must be positive\n");
                    exit(EXIT_FAILURE);
                }
                b = block_size; // 直接将b作为块大小（字节）
                break;
            }
            case 't':
				strcpy(t, optarg);
				break;
            case 'q':
                if(strcmp(optarg, "LRU") == 0){
                    policy = 0;
                }
                else if(strcmp(optarg, "2Q") == 0){
                    policy = 1;
                }
                else{
                    fprintf(stderr, "Error: Invalid replacement policy '%s'\n", optarg);
                    printUsage();
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                printUsage();
                return -1;
        }
    }
    
    // 检查必需的参数是否已提供
    if(s <= 0 || E <= 0 || b <= 0 || strlen(t) == 0)
    {
        fprintf(stderr, "Missing required command line argument\n");
        printUsage();
        return -1;
    }
    S = s; // 在这个lab里小s代表的就是set数量
    
    FILE* fp = fopen(t, "r"); // 打开trace文件
    if(fp == NULL)
	{
		printf("open error");
		exit(-1);
	}

    init_cache();  // 初始化cache
    parse_trace(); // 解析trace文件并模拟缓存行为

    // 输出结果
    printSummary(hit_count, miss_count, eviction_count);
    
    return 0;
}
