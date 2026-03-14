#ifndef OS_CONSOLE_H
#define OS_CONSOLE_H

#include <os/types.h>

void console_init(void);
void console_clear(void);
void console_write(const char *buf, u32 count);

#endif