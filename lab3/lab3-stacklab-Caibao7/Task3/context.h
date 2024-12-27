# ifndef CONTEXT_H
# define CONTEXT_H

#define ERR_THROW0 -1
#define ERR_GENEND -2
#define ERR_GENNIL -3
#define ERR_DIV0 -4

// Task 1

typedef char __ctx[120];

int __ctx_save(__ctx* dst);
void __ctx_restore(__ctx* src, int ret_val);

// Task 2

typedef struct __err_stk_node __err_stk_node;

struct __err_stk_node {
    __err_stk_node* prev;
    __ctx* ctx;
};

void __err_stk_push(__ctx* ctx);
__ctx* __err_stk_pop();

void __err_cleanup(const int* error);
int __check_err_stk();

# define try \
    int __err_try __attribute__((cleanup(__err_cleanup))) = 0; \
    __ctx __local_ctx; \
    __err_stk_push(&__local_ctx);\
    __err_try = __ctx_save(&__local_ctx);\
    if (__err_try == 0) 
    //

# define catch \
    else

#define throw(x) \
    do{ \
        while(__now_gen -> __err_stk_head == NULL){\
            __now_gen = __now_gen -> caller;\
        }\
        if (!__check_err_stk()){ \
            __ctx* __throw_ctx = __err_stk_pop(); \
            __ctx_restore(__throw_ctx, (x)); \
        } \
    } while(0);

// Task 3

typedef struct __generator __generator;

struct __generator {
    void (*f)(int);
    int data;
    __ctx ctx;
    void* stack;
    __generator* caller;
    __err_stk_node* __err_stk_head;
};

__generator* generator(void (*f)(int), int arg);
void generator_free(__generator** gen);

int send(__generator* gen, int value);
int yield(int value);

# endif