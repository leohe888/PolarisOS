#ifndef OS_STDLIB_H
#define OS_STDLIB_H

#include <os/types.h>

void delay(u32 count);
void hang(void);

u8 bcd_to_bin(u8 value);
u8 bin_to_bcd(u8 value);

#endif