[bits 32]

section .text

global inb
inb:
    push ebp            ; 保存调用者的栈帧基址
    mov ebp, esp        ; 设置当前函数的栈帧基址

    xor eax, eax
    mov edx, [ebp + 8]  ; port
    in al, dx           ; 从端口读入一个字节

    ; 一点点延迟
    jmp $ + 2
    jmp $ + 2
    jmp $ + 2

    leave               ; 等价于
                        ; mov esp, ebp  ; 释放局部变量
                        ; pop ebp       ; 恢复调用者的栈帧基址
    ret

global inw
inw:
    push ebp            ; 保存调用者的栈帧基址
    mov ebp, esp        ; 设置当前函数的栈帧基址

    xor eax, eax
    mov edx, [ebp + 8]  ; port
    in ax, dx           ; 从端口读入一个字节

    ; 一点点延迟
    jmp $ + 2
    jmp $ + 2
    jmp $ + 2

    leave               ; 等价于
                        ; mov esp, ebp  ; 释放局部变量
                        ; pop ebp       ; 恢复调用者的栈帧基址
    ret

global outb
outb:
    push ebp            ; 保存调用者的栈帧基址
    mov ebp, esp        ; 设置当前函数的栈帧基址

    xor eax, eax
    mov edx, [ebp + 8]  ; port
    mov eax, [ebp + 12] ; value
    out dx, al          ; 向端口写出一个字节

    ; 一点点延迟
    jmp $ + 2
    jmp $ + 2
    jmp $ + 2

    leave               ; 等价于
                        ; mov esp, ebp  ; 释放局部变量
                        ; pop ebp       ; 恢复调用者的栈帧基址
    ret

global outw
outw:
    push ebp            ; 保存调用者的栈帧基址
    mov ebp, esp        ; 设置当前函数的栈帧基址

    xor eax, eax
    mov edx, [ebp + 8]  ; port
    mov eax, [ebp + 12] ; value
    out dx, ax          ; 向端口写出一个字节

    ; 一点点延迟
    jmp $ + 2
    jmp $ + 2
    jmp $ + 2

    leave               ; 等价于
                        ; mov esp, ebp  ; 释放局部变量
                        ; pop ebp       ; 恢复调用者的栈帧基址
    ret
