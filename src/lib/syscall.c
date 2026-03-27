#include <os/syscall.h>
#include <os/types.h>

// - eax 存储系统调用号
// - ebx 存储第一个参数
// - ecx 存储第二个参数
// - edx 存储第三个参数
// - esi 存储第四个参数
// - edi 存储第五个参数
// - ebp 存储第六个参数
// - eax 存储返回值

static _inline u32 _syscall0(u32 nr)
{
    u32 ret;
    asm volatile(
        "int $0x80"
        : "=a"(ret)
        : "a"(nr)
    );
    return ret;
}


static _inline u32 _syscall1(u32 nr, u32 arg)
{
    u32 ret;
    asm volatile(
        "int $0x80\n"
        : "=a"(ret)
        : "a"(nr), "b"(arg));
    return ret;
}

static _inline u32 _syscall2(u32 nr, u32 arg1, u32 arg2)
{
    u32 ret;
    asm volatile(
        "int $0x80\n"
        : "=a"(ret)
        : "a"(nr), "b"(arg1), "c"(arg2));
    return ret;
}

static _inline u32 _syscall3(u32 nr, u32 arg1, u32 arg2, u32 arg3)
{
    u32 ret;
    asm volatile(
        "int $0x80\n"
        : "=a"(ret)
        : "a"(nr), "b"(arg1), "c"(arg2), "d"(arg3));
    return ret;
}

u32 test(void)
{
    return _syscall0(SYS_NR_TEST);
}

void exit(i32 status)
{
    _syscall1(SYS_NR_EXIT, (u32)status);
}

pid_t fork(void)
{
    return _syscall0(SYS_NR_FORK);
}

pid_t waitpid(pid_t pid, i32 *status)
{
    return _syscall2(SYS_NR_WAITPID, pid, (u32)status);
}

void yield(void)
{
    _syscall0(SYS_NR_YIELD);
}

void sleep(u32 ms)
{
    _syscall1(SYS_NR_SLEEP, ms);
}

pid_t getpid()
{
    return _syscall0(SYS_NR_GETPID);
}

pid_t getppid()
{
    return _syscall0(SYS_NR_GETPPID);
}

i32 brk(void *addr)
{
    return _syscall1(SYS_NR_BRK, (u32)addr);
}

i32 write(fd_t fd, const char *buf, u32 len)
{
    return _syscall3(SYS_NR_WRITE, fd, (u32)buf, len);
}

time_t time(void)
{
    return _syscall0(SYS_NR_TIME);
}
