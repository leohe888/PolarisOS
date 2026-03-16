#ifndef OS_INTERRUPT_H
#define OS_INTERRUPT_H

#include <os/types.h>

#define IDT_SIZE 256

// 门描述符
typedef struct gate_t
{
    u16 offset0;                    // 中断处理函数偏移地址第 0 ~ 15 位
    u16 selector;                   // 必须为代码段选择子
    u8 reserved;                    // 保留位，一般置为 0
    u8 type : 4;                    // 门类型。任务门 —— 0b0101，中断门 —— 0b1110，陷阱门 —— 0b1111
    u8 segment : 1;                 // 必须为 0，表示系统段
    u8 DPL : 2;                     // Descriptor Privilege Level —— 描述符特权等级。0（最高） ~ 3（最低）。只有 CPL ≤ 门的 DPL，才能触发这个门
    u8 present : 1;                 // 存在位。0 表示门无效，1 表示门有效
    u16 offset1;                    // 中断处理函数偏移地址第 16 ~ 31 位
} _packed gate_t;

typedef void *handler_t;            // 中断处理函数

void interrupt_init(void);

#endif