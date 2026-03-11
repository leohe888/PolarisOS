#include <os/os.h>

int magic = OS_MAGIC;
char message[] = "Hello, PolarisOS!";
char buf[1024];

void kernel_init()
{
    char *video = (char *)0xb8000;
    for (int i = 0; i < sizeof(message); i++)
    {
        video[i * 2] = message[i];
    }
}