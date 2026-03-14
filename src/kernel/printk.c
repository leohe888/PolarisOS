#include <os/printk.h>
#include <os/stdarg.h>
#include <os/stdio.h>
#include <os/console.h>

static char buf[1024];

int printk(const char *format, ...) {
    va_list args;
    int ret;

    va_start(args, format);
    
    ret = vsprintf(buf, format, args);

    va_end(args);

    console_write(buf, ret);

    return ret;
}