#ifndef OS_TYPES_H
#define OS_TYPES_H

#define EOF -1                          // End of file
#define EOS '\0'                        // End of string

#define NULL ((void *)0)                // 空指针

#define CONCAT(x, y) x##y               // 连接两个标识符
#define RESERVED_TOKEN(x, y) CONCAT(x, y)
#define RESERVED RESERVED_TOKEN(reserved, __LINE__)

#ifndef __cplusplus
#define bool _Bool
#define true 1
#define false 0
#endif

#define _packed __attribute__((packed))                 // 不使用内存对齐
#define _inline __attribute__((always_inline)) inline   // 内联函数

typedef unsigned int size_t;

typedef char i8;                        // 8 位有符号整数
typedef short i16;                      // 16 位有符号整数
typedef int i32;                        // 32 位有符号整数
typedef long long i64;                  // 64 位有符号整数

typedef unsigned char u8;               // 8 位无符号整数
typedef unsigned short u16;             // 16 位无符号整数  
typedef unsigned int u32;               // 32 位无符号整数
typedef unsigned long long u64;         // 64 位无符号整数

typedef i32 pid_t;
typedef i32 dev_t;

typedef u32 time_t;
typedef u32 idx_t;

typedef i32 fd_t;
typedef enum std_fd_t
{
    stdin,
    stdout,
    stderr,
} std_fd_t;

#endif