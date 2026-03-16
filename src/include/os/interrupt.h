#ifndef OS_INTERRUPT_H
#define OS_INTERRUPT_H

#include <os/types.h>

#define IDT_SIZE 256

#define IRQ_CLOCK 0                                     // 定时器
#define IRQ_KEYBOARD 1                                  // 键盘
#define IRQ_CASCADE 2                                   // 8259 从片控制器
#define IRQ_SERIAL_2 3                                  // 串口 2
#define IRQ_SERIAL_1 4                                  // 串口 1
#define IRQ_PARALLEL_2 5                                // 并口 2
#define IRQ_FLOPPY 6                                    // 软盘控制器
#define IRQ_PARALLEL_1 7                                // 并口 1
#define IRQ_RTC 8                                       // 实时时钟
#define IRQ_REDIRECT 9                                  // 重定向 IRQ2
#define IRQ_MOUSE 12                                    // 鼠标
#define IRQ_MATH 13                                     // 协处理器 x87
#define IRQ_HARDDISK 14                                 // ATA 硬盘第一通道
#define IRQ_HARDDISK2 15                                // ATA 硬盘第二通道

#define IRQ_MASTER_NR 0x20                              // 主片起始向量号
#define IRQ_SLAVE_NR 0x28                               // 从片起始向量号

// 门描述符
typedef struct gate_t
{
    u16 offset0;                                        // 中断处理函数偏移地址第 0 ~ 15 位
    u16 selector;                                       // 必须为代码段选择子
    u8 reserved;                                        // 保留位，一般置为 0
    u8 type : 4;                                        // 门类型。任务门 —— 0b0101，中断门 —— 0b1110，陷阱门 —— 0b1111
    u8 segment : 1;                                     // 必须为 0，表示系统段
    u8 DPL : 2;                                         // Descriptor Privilege Level —— 描述符特权等级。0（最高） ~ 3（最低）。只有 CPL ≤ 门的 DPL，才能触发这个门
    u8 present : 1;                                     // 存在位。0 表示门无效，1 表示门有效
    u16 offset1;                                        // 中断处理函数偏移地址第 16 ~ 31 位
} _packed gate_t;

typedef void *handler_t;                                // 中断处理函数

void send_eoi(int vector);

void set_interrupt_handler(u32 irq, handler_t handler); // 设置中断处理函数

void set_interrupt_mask(u32 irq, bool enable);          // 设置中断屏蔽字

#endif