[org 0x1000]                ; 告诉汇编器本程序被加载到内存地址 0x1000

; loader 魔数
dw 0xaa55

; 打印加载信息
mov si, load_msg
call print_string

; 检测物理内存
detect_memory:
    xor ebx, ebx            ; 首次调用 ebx = 0，后续用返回的 ebx 继续调用，直到 ebx = 0

    mov ax, 0
    mov es, ax
    mov di, ards_buf        ; es:di -> ARDS 缓冲区

    mov edx, 0x534d4150     ; 'SMAP' 的 ASCII 码

.next:
    mov ecx, 20         ; ARDS 缓冲区大小

    mov eax, 0xe820     ; 获取物理内存映射
    int 0x15            ; 调用 BIOS 杂项系统服务中断

    jc .error            ; 调用失败会设置 CF 标志

    add di, 20          ; ARDS 缓冲区指针后移

    inc word [ards_cnt] ; ARDS 计数器加 1

    cmp ebx, 0
    jnz .next

    ; 打印检测信息
    mov si, detect_msg
    call print_string

    jmp prepare_protected_mode

.error:
    mov si, error_msg
    call print_string
.hlt:
    hlt
    jmp .hlt

; 准备保护模式
prepare_protected_mode:
    ; 关闭中断
    cli

    ; 打开 A20 地址线
    in al, 0x92             ; 读取 0x92 端口
    or al, 0x02             ; 将第 1 位设置为 1
    out 0x92, al            ; 写回 0x92 端口

    lgdt [gdt_ptr]          ; 加载 GDT

    ; 开启保护模式
    mov eax, cr0
    or eax, 1               ; 设置 PE 位（CR0 的第 0 位）
    mov cr0, eax

    ; 远跳转刷新流水线，进入保护模式
    jmp CODE_SELECTOR:protected_mode

; ===============================
; 功能：打印字符串
; 传入参数：si —— 字符串地址
; 传出参数：无
; ===============================
print_string:
    mov ah, 0x0e
.next:
    mov al, [si]
    cmp al, 0
    jz .done
    int 0x10
    inc si
    jmp .next
.done:
    ret

; 加载信息
load_msg:
    db "Loading PolarisOS...", 13, 10, 0
                            ; \r\n\0

; 错误信息
error_msg:
    db "Loading Error!!!", 13, 10, 0
                            ; \r\n\0

; 检测信息
detect_msg:
    db "Detecting Memory Success...", 10, 13, 0
                            ; \r\n\0

[bits 32]                   ; 告诉汇编器接下来生成 32 位代码
protected_mode:
    ; 设置段寄存器
    mov ax, DATA_SELECTOR
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; 设置栈顶指针
    mov esp, 0x10000

    ; 读取内核
    mov edi, 0x10000        ; 目标存放地址
    mov ecx, 10             ; 起始扇区号
    mov bl, 200             ; 读取的扇区数量
    call read_disk

    ; 跳转到内核
    jmp CODE_SELECTOR:0x10000

    ud2                     ; 不可能执行到这里，如果执行到这里说明发生了错误，触发 ud2 异常

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

; 段选择子：
; 位 0 ~ 1：RPL —— Request Privilege Level，请求特权级
; 位 2：TI —— Table Indicator，表指示符，0 表示 GDT，1 表示 LDT
; 位 3 ~ 15：GDT 索引
CODE_SELECTOR equ (1 << 3)      ; 代码段选择子
DATA_SELECTOR equ (2 << 3)      ; 数据段选择子

MEMORY_BASE equ 0               ; 内存基地址
MEMORY_LIMIT equ (4 * 1024 * 1024 * 1024) / (4 * 1024) - 1
                                ; 内存界限，4GB / 4KB - 1

; GDT 指针
gdt_ptr:
    dw gdt_end - gdt_base - 1   ; GDT 界限
    dd gdt_base                 ; GDT 基地址

; GDT
gdt_base:
    dd 0, 0                     ; NULL 描述符
gdt_code:
    dw MEMORY_LIMIT & 0xffff                        ; 段界限第 0 ~ 15 位
    dw MEMORY_BASE & 0xffff                         ; 段基地址第 0 ~ 15 位
    db (MEMORY_BASE >> 16) & 0xff                   ; 段基地址第 16 ~ 23 位
    db 0b_1_00_1_1_0_1_0                            ; 存在、DPL 0、应用段、代码段、非一致代码段、可读、未被 CPU 访问
    db 0b_1_1_0_0_0000 | (MEMORY_LIMIT >> 16) & 0xf ; 粒度 4KB、32 位、不是 64 位、段界限第 16 ~ 19 位
    db (MEMORY_BASE >> 24) & 0xff                   ; 段基地址第 24 ~ 31 位
gdt_data:
    dw MEMORY_LIMIT & 0xffff                        ; 段界限第 0 ~ 15 位
    dw MEMORY_BASE & 0xffff                         ; 段基地址第 0 ~ 15 位
    db (MEMORY_BASE >> 16) & 0xff                   ; 段基地址第 16 ~ 23 位
    db 0b_1_00_1_0_0_1_0                            ; 存在、DPL 0、应用段、数据段、向上扩展、可读写、未被 CPU 访问
    db 0b_1_1_0_0_0000 | (MEMORY_LIMIT >> 16) & 0xf ; 粒度 4KB、32 位、不是 64 位、段界限第 16 ~ 19 位
    db (MEMORY_BASE >> 24) & 0xff                   ; 段基地址第 24 ~ 31 位
gdt_end:

; ARDS 计数器
ards_cnt:
    dw 0

; ARDS 缓冲区
ards_buf: