#include <os/assert.h>
#include <os/stdarg.h>
#include <os/types.h>
#include <os/stdio.h>
#include <os/printk.h>

static u8 buf[1024];

// 无限循环
static void spin(char *name)
{
    printk("spinning in %s ...\n", name);
    while (true)
        ;
}

// 断言失败
void assertion_failure(const char *exp, char *file, char *base, int line)
{
    printk(
        "\n--> assert(%s) failed!!!\n"
        "--> file: %s \n"
        "--> base: %s \n"
        "--> line: %d \n",
        exp, file, base, line);

    spin("assertion_failure()");

    // 不可能执行到这里，如果执行到这里说明发生了错误，触发 ud2 异常
    asm volatile("ud2");
}

void panic(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int ret = vsprintf(buf, fmt, args);
    va_end(args);

    printk("!!! panic !!!\n--> %s \n", buf);
    spin("panic()");

    // 不可能执行到这里，如果执行到这里说明发生了错误，触发 ud2 异常
    asm volatile("ud2");
}