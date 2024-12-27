    .text
    .globl __ctx_save
    __ctx_save:
        endbr64
	    movq %rsp, (0)(%rdi)
	    movq %rbp, (8)(%rdi)
	    movq %r12, (16)(%rdi)
	    movq %r13, (24)(%rdi)
	    movq %r14, (32)(%rdi)
	    movq %r15, (40)(%rdi)
        movq %rbx, (48)(%rdi)
	    movq (%rsp), %rdx		
	    movq %rdx, (56)(%rdi)
		movq %rsi, (64)(%rdi)
		movq %rdi, (72)(%rdi)
	    movq $0, %rax
	ret


    .globl __ctx_restore
    __ctx_restore:
        endbr64
        movq (0)(%rdi), %rsp
	    movq (8)(%rdi), %rbp
	    movq (16)(%rdi), %r12
	    movq (24)(%rdi), %r13
	    movq (32)(%rdi), %r14
	    movq (40)(%rdi), %r15
        movq (48)(%rdi), %rbx
		movl %esi, %eax 
        movq (56)(%rdi), %rdx
	    movq %rdx, (%rsp)
		movq (64)(%rdi), %rsi
		movq (72)(%rdi), %rdi
	ret

