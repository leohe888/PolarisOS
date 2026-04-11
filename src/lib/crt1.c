#include <os/types.h>
#include <os/syscall.h>
#include <os/string.h>

int main(int argc, char **argv, char **envp);

// libc 构造函数
weak void _init()
{
}

// libc 析构函数
weak void _fini()
{
}

int __libc_start_main(
    int (*main)(int argc, char **argv, char **envp),
    int argc, char **argv,
    void (*_init)(),
    void (*_fini)(),
    void (*ldso)(), // 动态链接器
    void *stack_end)
{
    char **envp = argv + argc + 1;  // argv 是一个指针，指向一个指针数组，argv + argc 就是从 argv 开始向后移动 argc 个元素，由于最后一个元素是 NULL，所以还需要再移动一个元素才是 envp 的位置
    _init();
    int i = main(argc, argv, envp);
    _fini();
    exit(i);
}
