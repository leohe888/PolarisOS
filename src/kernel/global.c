#include <os/global.h>
#include <os/string.h>
#include <os/debug.h>

descriptor_t gdt[GDT_SIZE]; // 内核 GDT
pointer_t gdt_ptr;          // 内核 GDT 指针

// 初始化内核 GDT
void gdt_init(void)
{
    DEBUGK("init gdt!!!\n");

    asm volatile("sgdt gdt_ptr");

    memcpy(&gdt, (void *)gdt_ptr.base, gdt_ptr.limit + 1);

    gdt_ptr.base = (u32)&gdt;
    gdt_ptr.limit = sizeof(gdt) - 1;
    asm volatile("lgdt gdt_ptr");
}
