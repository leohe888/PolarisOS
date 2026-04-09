#ifndef OS_STDLIB_H
#define OS_STDLIB_H

#include <os/types.h>

#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

void delay(u32 count);
void hang(void);

u8 bcd_to_bin(u8 value);
u8 bin_to_bcd(u8 value);

u32 div_round_up(u32 dividend, u32 divisor);

int atoi(const char *str);

#endif