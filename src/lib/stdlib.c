#include <os/stdlib.h>

void delay(u32 count)
{
    while (count--)
        ;
}

void hang(void)
{
    while (true)
        ;
}
