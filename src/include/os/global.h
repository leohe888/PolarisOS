#ifndef OS_GLOBAL_H
#define OS_GLOBAL_H

#include <os/types.h>

#define GDT_SIZE 128

// 全局描述符
typedef struct descriptor_t
{
    unsigned short limit_low;       // 段界限第 0 ~ 15 位
    unsigned int base_low : 24;     // 段基地址第 0 ~ 23 位
    unsigned char type : 4;         // 段类型
    unsigned char segment : 1;      // 0 表示系统段，1 表示应用段
    unsigned char DPL : 2;          // Descriptor Privilege Level —— 描述符特权等级。0（最高） ~ 3（最低）
    unsigned char present : 1;      // 存在位。0 表示不存在，1 表示存在
    unsigned char limit_high : 4;   // 段界限第 16 ~ 19
    unsigned char available : 1;    // 操作系统保留位。一般置为 0
    unsigned char long_mode : 1;    // 0 表示 32 位，1 表示 64 位
    unsigned char big : 1;          // 0 表示 16 位，1 表示 32 位
    unsigned char granularity : 1;  // 粒度。0 表示 1B，1 表示 4KB
    unsigned char base_high;        // 段基地址第 24 ~ 31 位
} _packed descriptor_t;

// 段选择子
typedef struct selector_t
{
    u8 RPL : 2;                     // Request Privilege Level —— 请求特权级。0（最高） ~ 3（最低）
    u8 TI : 1;                      // Table Indicator —— 表指示位。0 表示 GDT，1 表示 LDT
    u16 index : 13;                 // GDT 索引
} selector_t;

// GDT 指针
typedef struct pointer_t
{
    u16 limit;                      // GDT 界限
    u32 base;                       // GDT 基地址
} _packed pointer_t;

void gdt_init(void);

#endif