#ifndef OS_IO_H
#define OS_IO_H

#include <os/types.h>

u8 inb(u16 port);               // 从端口读入一个字节
u16 inw(u16 port);              // 从端口读入一个字

void outb(u16 port, u8 value);  // 向端口写出一个字节
void outw(u16 port, u16 value); // 向端口写出一个字

#endif
