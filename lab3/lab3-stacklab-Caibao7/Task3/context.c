#include "context.h"
#include "assert.h"
#include "stdlib.h"
#include "string.h"

#define STACK_SIZE (4096)

__generator __main_gen;
__generator* __now_gen = &__main_gen;

// Task 2

void __err_stk_push(__ctx* ctx){
    assert(ctx != 0);
    __err_stk_node *newnode = (__err_stk_node *)malloc(sizeof(__err_stk_node));
    newnode -> prev = __now_gen->__err_stk_head;
    newnode -> ctx = ctx;
    __now_gen->__err_stk_head = newnode;
}

__ctx* __err_stk_pop(){
    assert(__now_gen->__err_stk_head != 0);
    __err_stk_node *top = __now_gen->__err_stk_head;
    __now_gen->__err_stk_head = top -> prev;
    __ctx* tmp = top -> ctx;
    free(top);
    return tmp;
}

void __err_cleanup(const int* error) {
    if (*error == 0) __err_stk_pop();
}

int __check_err_stk(){
    return __now_gen->__err_stk_head == 0;
}

// Task 3

// 跳板函数
static void plank(void(*f)(int), int arg) {
    f(arg);
    throw(ERR_GENEND);
}

int send(__generator* gen, int value) {
    if (gen == 0) throw(ERR_GENNIL);
    
    gen->data = value;
    int ret_val = __ctx_save(&__now_gen->ctx);
    if (ret_val == 0) { 
        __now_gen = gen;
        __ctx_restore(&(__now_gen->ctx), 1); 
    } 
    return gen->data;
}

int yield(int value) {
    if (__now_gen->caller == 0) throw(ERR_GENNIL);
    __now_gen->data = value;
    int ret_val = __ctx_save(&__now_gen->ctx);
    if (ret_val == 0) {
        __now_gen = __now_gen->caller;
        __ctx_restore(&(__now_gen->ctx), 1);
    }
    return __now_gen->data;
}

__generator* generator(void (*f)(int), int arg) {
    __generator* gen = (__generator*) malloc(sizeof(__generator));
    if (gen == NULL) return NULL;
    gen->f = f; 
    gen->data = 0;
    gen->caller = __now_gen;
    gen->__err_stk_head = NULL;
    gen->stack = malloc(STACK_SIZE);

    // 初始化栈
    char* stack_bottom = (char*)gen->stack + STACK_SIZE;

    // 将栈指针对齐到16字节
    stack_bottom= (char*)((unsigned long)stack_bottom & ~0xF);

    // 定义一个结构体来映射__ctx数组
    typedef struct __ctx_struct {
        unsigned long rsp;  // 从0开始，每个占8字节
        unsigned long rbp;  // 8
        unsigned long r12;  // 16
        unsigned long r13;  // 24
        unsigned long r14;  // 32
        unsigned long r15;  // 40
        unsigned long rbx;  // 48
        unsigned long ret;  // 56
        unsigned long rsi;  // 64
        unsigned long rdi;  // 72
    } __attribute__((packed)) __ctx_struct;

    // 初始化生成器的上下文
    __ctx_struct* ctx = (__ctx_struct*)gen->ctx;

    ctx->rsp = (unsigned long)stack_bottom;
    ctx->rbp = (unsigned long)stack_bottom;
    ctx->r12 = 0;
    ctx->r13 = 0;
    ctx->r14 = 0;
    ctx->r15 = 0;
    ctx->rbx = 0;
    ctx->ret = (unsigned long)plank;
    ctx->rdi = (unsigned long)f;
    ctx->rsi = (unsigned long)arg;
    return gen;
}

void generator_free(__generator** gen) {
    if (*gen == NULL) throw(ERR_GENNIL);
    while ((*gen)->__err_stk_head != 0) {
        __err_stk_node* tmp = (*gen)->__err_stk_head;
        (*gen)->__err_stk_head = (*gen)->__err_stk_head->prev;
        free(tmp);
    }
    free(*gen);
    *gen = NULL;
}