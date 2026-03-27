[bits 32]

; multiboot2 头部必须位于内核镜像前 32768 字节内，8 字节对齐，包含标准起始结构 + 可选标签 + 结束标签，校验和必须使整个头部结构的 32 位总和为 0
magic   equ 0xe85250d6
i386    equ 0
length  equ header_end - header_start

section .multiboot2
header_start:
    dd magic                    ; 魔数
    dd i386                     ; 32位保护模式
    dd length                   ; 头部长度
    dd -(magic + i386 + length) ; 校验和

    ; 结束标记
    dw 0                        ; type
    dw 0                        ; flags
    dd 8                        ; size
header_end:

extern console_init
extern gdt_init
extern memory_init
extern kernel_init

extern gdt_ptr

CODE_SELECTOR equ (1 << 3)      ; 代码段、GDT、RPL 0
DATA_SELECTOR equ (2 << 3)      ; 数据段、GDT、RPL 0

section .text
global _start
_start:
    ; 保存 bootloader 传递的参数
    push ebx
    push eax

    call console_init       ; 初始化控制台
    call gdt_init           ; 重新初始化 GDT
    lgdt [gdt_ptr]          ; 加载 GDT
    jmp CODE_SELECTOR:.flush

.flush:
    ; 刷新段寄存器
    mov ax, DATA_SELECTOR
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    call memory_init    ; 初始化内存
    mov esp, 0x10000    ; 重新设置栈顶
    call kernel_init    ; 初始化内核

    ; 无限循环
    jmp $