#include <os/interrupt.h>
#include <os/syscall.h>
#include <os/debug.h>
#include <os/task.h>
#include <os/stdio.h>
#include <os/arena.h>
#include <os/fs.h>
#include <os/string.h>

#define LOGK(fmt, args...) DEBUGK(fmt, ##args)

void idle_thread(void)
{
    set_interrupt_state(true);
    u32 counter = 0;
    while (true)
    {
        // LOGK("idle task.... %d\n", counter++);
        asm volatile(
            "sti\n" // 开中断
            "hlt\n" // 关闭 CPU，进入暂停状态，等待外中断的到来
        );
        yield(); // 放弃执行权，调度执行其他任务
    }
}

void test_recursion(void)
{
    char tmp[0x400];
    test_recursion();
}

void posh_main();

static void user_init_thread()
{
    while (true)
    {   
        u32 status;
        pid_t pid = fork();
        if (pid)
        {
            pid_t child = waitpid(pid, &status);
            printf("wait pid %d status %d %d\n", child, status, time());
        }
        else
        {
            posh_main();
        }
    }
}


void dev_init();

void init_thread(void)
{
    char temp[100]; // 为栈顶有足够的空间
    dev_init();
    task_to_user_mode(user_init_thread);
}

void test_thread(void)
{
    set_interrupt_state(true);

    while (true)
    {
        // test();
        sleep(10);
    }
}