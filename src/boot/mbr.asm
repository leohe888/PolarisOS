[org 0x7c00]                ; 告诉汇编器本程序被加载到内存地址 0x7c00（BIOS 规定的 MBR 加载地址）

; 设置显示器模式为 80×25 16 色文本模式，并清空屏幕
mov ah, 0x00                ; 设置显示器模式
mov al, 0x03                ; 80×25 16 色文本模式
int 0x10                    ; 调用 BIOS 显示服务中断

; 初始化段寄存器
mov ax, 0
mov ds, ax
mov es, ax
mov ss, ax
mov sp, 0x7c00

; 打印引导信息
mov si, boot_msg
call print_string

; 读取 loader
mov edi, 0x1000             ; 目标存放地址
mov ecx, 2                  ; 起始扇区号
mov bl, 4                   ; 读取的扇区数量
call read_disk

; 检查 loader 的魔数
cmp word [0x1000], 0xaa55
jnz error

; 跳转到 loader
jmp 0x0000:0x1002

error:
    mov si, error_msg
    call print_string
.hlt:
    hlt
    jmp .hlt

; ===============================
; 功能：打印字符串
; 传入参数：si —— 字符串地址
; 传出参数：无
; ===============================
print_string:
    mov ah, 0x0e            ; 显示字符
.next:
    mov al, [si]            ; 待显示的字符
    cmp al, 0
    jz .done
    int 0x10                ; 调用 BIOS 显示服务中断
    inc si
    jmp .next
.done:
    ret

; ===============================
; 功能：从磁盘读取数据
; 传入参数：edi —— 目标存放地址，ecx —— 起始扇区号，bl —— 扇区数量
; 传出参数：无
; 注意：仅支持读取主通道的主盘
; ===============================
read_disk:
    mov dx, 0x1f2           ; 0x1f2 —— 读取的扇区数量
    mov al, bl
    out dx, al

    inc dx                  ; 0x1f3 —— 起始扇区号第 0 ~ 7 位
    mov al, cl
    out dx, al

    inc dx                  ; 0x1f4 —— 起始扇区号第 8 ~ 15 位
    shr ecx, 8
    mov al, cl
    out dx, al

    inc dx                  ; 0x1f5 —— 起始扇区号第 16 ~ 23 位
    shr ecx, 8
    mov al, cl
    out dx, al

    inc dx                  ; 0x1f6
                            ; 位 7：固定为 1
                            ; 位 6：0 表示 CHS 模式，1 表示 LBA 模式
                            ; 位 5：固定为 1
                            ; 位 4：0 表示主盘，1 表示从盘
                            ; 位 0 ~ 3：起始扇区号第 24 ~ 27 位
    shr ecx, 8
    and cl, 0x0f
    mov al, 0b1110_0000
    or al, cl
    out dx, al

    inc dx                  ; 0x1f7 —— 命令
    mov al, 0x20            ; 读命令
    out dx, al

    xor ecx, ecx
    mov cl, bl

.read:
    push cx
    call .waits             ; 等待磁盘准备好
    call .reads             ; 读取一个扇区
    pop cx
    loop .read
    ret

.waits:
    mov dx, 0x1f7           ; 0x1f7 —— 状态
                            ; 位 7：BSY —— Busy，磁盘忙
                            ; 位 3：DRQ —— Data Request，数据准备好
.check:
    in al, dx

    ; 一点点延迟
    jmp $ + 2
    jmp $ + 2
    jmp $ + 2

    and al, 0b1000_1000
    cmp al, 0b0000_1000
    jnz .check
    ret

.reads:
    mov dx, 0x1f0           ; 0x1f0 —— 数据
    mov cx, 256             ; 一个扇区有 256 个字
.readw:
    in ax, dx

    ; 一点点延迟
    jmp $ + 2
    jmp $ + 2
    jmp $ + 2

    mov [edi], ax
    add edi, 2
    loop .readw
    ret

; 引导信息
boot_msg:
    db "booting PolarisOS...", 13, 10, 0
                            ; \r\n\0

; 错误信息
error_msg:
    db "booting error!!!", 13, 10, 0
                            ; \r\n\0

; 填充剩余空间
times 510 - ($ - $$) db 0

; MBR 魔数，BIOS 会检查这个魔数
dw 0xaa55
