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
#include <os/fs.h>

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

    device_t *serial = device_find(DEV_SERIAL, 0);
    assert(serial);

    device_t *keyboard = device_find(DEV_KEYBOARD, 0);
    assert(keyboard);

    device_t *console = device_find(DEV_CONSOLE, 0);
    assert(console);

    // device_read(serial->dev, &ch, 1, 0, 0);
    device_read(keyboard->dev, &ch, 1, 0, 0);

    device_write(serial->dev, &ch, 1, 0, 0);
    device_write(console->dev, &ch, 1, 0, 0);

    return 255;
}

void sys_execve();

fd_t sys_dup();
fd_t sys_dup2();

int sys_pipe();

int sys_read();
int sys_write();
int sys_lseek();
int sys_readdir();

fd_t sys_open();
fd_t sys_creat();
void sys_close();

int sys_chdir();
int sys_chroot();
char *sys_getcwd();

int sys_mkdir();
int sys_rmdir();

int sys_link();
int sys_unlink();

time_t sys_time();
mode_t sys_umask();

void console_clear();

int sys_stat();
int sys_fstat();

int sys_mknod();

int sys_mount();
int sys_umount();

int sys_mkfs();

int sys_brk();
int sys_mmap();
int sys_munmap();

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

    syscall_table[SYS_NR_EXECVE] = sys_execve;

    syscall_table[SYS_NR_SLEEP] = task_sleep;
    syscall_table[SYS_NR_YIELD] = task_yield;

    syscall_table[SYS_NR_GETPID] = sys_getpid;
    syscall_table[SYS_NR_GETPPID] = sys_getppid;

    syscall_table[SYS_NR_BRK] = sys_brk;
    syscall_table[SYS_NR_MMAP] = sys_mmap;
    syscall_table[SYS_NR_MUNMAP] = sys_munmap;

    syscall_table[SYS_NR_DUP] = sys_dup;
    syscall_table[SYS_NR_DUP2] = sys_dup2;

    syscall_table[SYS_NR_PIPE] = sys_pipe;

    syscall_table[SYS_NR_READ] = sys_read;
    syscall_table[SYS_NR_WRITE] = sys_write;

    syscall_table[SYS_NR_MKDIR] = sys_mkdir;
    syscall_table[SYS_NR_RMDIR] = sys_rmdir;

    syscall_table[SYS_NR_OPEN] = sys_open;
    syscall_table[SYS_NR_CREAT] = sys_creat;
    syscall_table[SYS_NR_CLOSE] = sys_close;
    syscall_table[SYS_NR_LSEEK] = sys_lseek;
    syscall_table[SYS_NR_READDIR] = sys_readdir;

    syscall_table[SYS_NR_LINK] = sys_link;
    syscall_table[SYS_NR_UNLINK] = sys_unlink;

    syscall_table[SYS_NR_TIME] = sys_time;

    syscall_table[SYS_NR_UMASK] = sys_umask;

    syscall_table[SYS_NR_CHDIR] = sys_chdir;
    syscall_table[SYS_NR_CHROOT] = sys_chroot;
    syscall_table[SYS_NR_GETCWD] = sys_getcwd;

    syscall_table[SYS_NR_CLEAR] = console_clear;

    syscall_table[SYS_NR_STAT] = sys_stat;
    syscall_table[SYS_NR_FSTAT] = sys_fstat;

    syscall_table[SYS_NR_MKNOD] = sys_mknod;

    syscall_table[SYS_NR_MOUNT] = sys_mount;
    syscall_table[SYS_NR_UMOUNT] = sys_umount;

    syscall_table[SYS_NR_MKFS] = sys_mkfs;
}