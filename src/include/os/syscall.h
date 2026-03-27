#ifndef OS_SYSCALL_H
#define OS_SYSCALL_H

#include <os/types.h>

typedef enum syscall_t
{
    SYS_NR_TEST,
    SYS_NR_EXIT = 1,
    SYS_NR_FORK = 2,
    SYS_NR_WRITE = 4,
    SYS_NR_WAITPID = 7,
    SYS_NR_TIME = 13,
    SYS_NR_GETPID = 20,
    SYS_NR_BRK = 45,
    SYS_NR_GETPPID = 64,
    SYS_NR_YIELD = 158,
    SYS_NR_SLEEP = 162,
} syscall_t;

u32 test(void);

pid_t fork(void);
void exit(i32 status);
pid_t waitpid(pid_t pid, i32 *status);

void yield(void);
void sleep(u32 ms);

pid_t getpid(void);
pid_t getppid(void);

i32 brk(void *addr);

i32 write(fd_t fd, const char *buf, u32 len);

time_t time(void);

#endif