#include <os/types.h>
#include <os/stdlib.h>
#include <os/interrupt.h>

void tss_init(void);
void memory_map_init(void);
void mapping_init(void);
void arena_init(void);
void interrupt_init(void);
void clock_init(void);
void keyboard_init(void);
void time_init(void);
void rtc_init(void);
void task_init(void);
void syscall_init(void);

void kernel_init()
{
    tss_init();
    memory_map_init();
    mapping_init();
    arena_init();
    
    interrupt_init();
    clock_init();
    keyboard_init();

    time_init();
    // rtc_init();

    task_init();
    syscall_init();

    set_interrupt_state(true);
}
