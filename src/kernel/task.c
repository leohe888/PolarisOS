#include <os/task.h>
#include <os/printk.h>

#define PAGE_SIZE 0x1000    // 4KB

task_t *a = (task_t *)0x1000;
task_t *b = (task_t *)0x2000;

void task_switch(task_t *next);

task_t *running_task(void)
{
    asm volatile(
        "movl %esp, %eax\n"
        "andl $0xfffff000, %eax\n"
    );
}

void schedule(void)
{
    task_t *current = running_task();
    task_t *next = current == a ? b : a;
    task_switch(next);
}

void thread_a(void)
{
    asm volatile ("sti");

    while (true)
    {
        printk("A");
    }
}

void thread_b(void)
{
    asm volatile ("sti");
    
    while (true)
    {
        printk("B");
    }
}


static void task_create(task_t *task, target_t target)
{
    u32 stack = (u32)task + PAGE_SIZE;

    stack -= sizeof(task_frame_t);
    task_frame_t *frame = (task_frame_t *)stack;
    frame->ebx = 0x11111111;
    frame->esi = 0x22222222;
    frame->edi = 0x33333333;
    frame->ebp = 0x44444444;
    frame->eip = target;

    task->stack = (u32 *)stack;
}

void task_init(void)
{
    task_create(a, thread_a);
    task_create(b, thread_b);
    schedule();
}