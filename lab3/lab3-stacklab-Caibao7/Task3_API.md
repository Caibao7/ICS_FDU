# API Guideline for Task 3

## __ctx

A type that represents 120 bytes of space.

## struct __err_stk_node

### Member variables

#### struct __err_stk_node* prev

The address of the previous item in the list.

#### __ctx* ctx

The address of the current saved context.

## struct __generator

### Member variables

#### void (*f)(int)

The function to be executed by the generator.

#### int data

Used to pass data between calls to generators and functions and generators.
It becomes the return value of `send` by assigning a value to this variable by keyword `yield`, and becomes the return value of `yield` by assigning a value to this variable by keyword `send`.

#### __ctx ctx

The context of this generator.

#### void* stack

The start(lower) address of the generator's stack space.

#### __generator* caller

The generator which calls the `send` function to activate this generator.

#### __err_stk_node* __err_stk_head

The exception handling stack of this generator.

## int __ctx_save(__ctx* dst)

Save the context now into the address referred by `dst`, and return 0.

## void __ctx_restore(__ctx* src, int ret_val)

Restore the context which is saved in src to the registers and cause `__ctx_save` to return value `ret_val`.

## void __err_stk_push(__ctx* ctx)

Push `ctx` into the exception handling stack of the generator now.

## __ctx* __err_stk_pop()

Pop the top element in the exception handling stack of the generator now.

## void __err_cleanup(const int* error)

Things to be done at the end of the variable's life cycle.