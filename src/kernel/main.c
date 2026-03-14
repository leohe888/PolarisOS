#include <os/os.h>
#include <os/console.h>
#include <os/assert.h>

void kernel_init()
{
    console_init();
    // assert(3 < 5);
    assert(3 > 5);
    panic("Out of Memory");
}
