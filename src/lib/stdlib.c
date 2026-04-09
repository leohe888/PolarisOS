#include <os/stdlib.h>

// 延时
void delay(u32 count)
{
    while (count--)
        ;
}

// 死循环
void hang(void)
{
    while (true)
        ;
}

// 将 bcd 码转成整数
u8 bcd_to_bin(u8 value)
{
    return (value & 0xf) + (value >> 4) * 10;
}

// 将整数转成 bcd 码
u8 bin_to_bcd(u8 value)
{
    return (value / 10) * 0x10 + (value % 10);
}

// 除法 —— 向上取整，只要有余数就进 1
u32 div_round_up(u32 dividend, u32 divisor)
{
    return (dividend + divisor - 1) / divisor;
}

int atoi(const char *str)
{
    if (str == NULL)
        return 0;
    bool sign = 1;
    int result = 0;
    if (*str == '-')
    {
        sign = -1;
        str++;
    }
    for (; *str; str++)
    {
        result = result * 10 + (*str - '0');
    }
    return result;
}