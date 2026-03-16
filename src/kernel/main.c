#include <os/os.h>
#include <os/console.h>
#include <os/global.h>
#include <os/task.h>
#include <os/interrupt.h>
#include <os/debug.h>
#include <os/stdlib.h>

void kernel_init()
{
    console_init();
    gdt_init();
    interrupt_init();
    task_init();
}
