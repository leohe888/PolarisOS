[bits 32]

extern kernel_init

global _start
_start:
    ; 初始化内核
    call kernel_init

    ; 无限循环
    jmp $