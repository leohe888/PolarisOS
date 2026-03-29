#include <os/interrupt.h>
#include <os/assert.h>
#include <os/debug.h>
#include <os/syscall.h>
#include <os/task.h>
#include <os/console.h>
#include <os/memory.h>
#include <os/device.h>
#include <os/string.h>
#include <os/buffer.h>

#define LOGK(fmt, args...) DEBUGK(fmt, ##args)

#define SYSCALL_SIZE 256

handler_t syscall_table[SYSCALL_SIZE];

void syscall_check(u32 nr)
{
    if (nr >= SYSCALL_SIZE)
    {
        panic("syscall nr error!!!");
    }
}

static void sys_default(void)
{
    panic("syscall not implemented!!!");
}

static u32 sys_test(void)
{
    char ch;
    device_t *device;

    device = device_find(DEV_IDE_DISK, 0);
    assert(device);

    buffer_t *buf = bread(device->dev, 0); // 读取主引导块

    char *data = buf->data + SECTOR_SIZE;
    memset(data, 0x5a, SECTOR_SIZE);

    buf->dirty = true;

    brelse(buf);

    return 255;
}

i32 console_write(void *dev, char *buf, u32 count);

i32 sys_write(fd_t fd, const char *buf, u32 len)
{
    if (fd == stdout || fd == stderr)
    {
        return console_write(NULL, buf, len);
    }
    panic("not implemented!!!");
    return 0;
}

time_t sys_time(void);

void syscall_init(void)
{
    for (size_t i = 0; i < SYSCALL_SIZE; i++)
    {
        syscall_table[i] = sys_default;
    }

    syscall_table[SYS_NR_TEST] = sys_test;

    syscall_table[SYS_NR_EXIT] = task_exit;
    syscall_table[SYS_NR_FORK] = task_fork;
    syscall_table[SYS_NR_WAITPID] = task_waitpid;

    syscall_table[SYS_NR_SLEEP] = task_sleep;
    syscall_table[SYS_NR_YIELD] = task_yield;

    syscall_table[SYS_NR_GETPID] = sys_getpid;
    syscall_table[SYS_NR_GETPPID] = sys_getppid;

    syscall_table[SYS_NR_BRK] = sys_brk;

    syscall_table[SYS_NR_WRITE] = sys_write;

    syscall_table[SYS_NR_TIME] = sys_time;
}