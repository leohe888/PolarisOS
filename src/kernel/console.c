#include <os/console.h>
#include <os/io.h>
#include <os/string.h>

#define CRT_ADDR_REG 0x3D4              // CRT 控制器索引寄存器
#define CRT_DATA_REG 0x3D5              // CRT 控制器数据寄存器

#define CRT_START_ADDR_H 0xC            // 显示起始地址高 8 位
#define CRT_START_ADDR_L 0xD            // 显示起始地址低 8 位
#define CRT_CURSOR_H 0xE                // 光标位置高 8 位
#define CRT_CURSOR_L 0xF                // 光标位置低 8 位

#define MEM_BASE 0xB8000                // 文本模式显存起始地址 —— 0xB8000
#define MEM_SIZE 0x8000                 // 文本模式显卡内存大小 —— 32KB
#define MEM_END (MEM_BASE + MEM_SIZE)   // 文本模式显存结束地址 —— 0xBFFFF
#define WIDTH 80                        // 屏幕文本列数
#define HEIGHT 25                       // 屏幕文本行数
#define ROW_SIZE (WIDTH * 2)            // 每行字节数
#define SCR_SIZE (ROW_SIZE * HEIGHT)    // 屏幕字节数

#define ASCII_NUL 0x00
#define ASCII_ENQ 0x05
#define ASCII_BEL 0x07                  // \a   —— 响铃
#define ASCII_BS 0x08                   // \b   —— 退格
#define ASCII_HT 0x09                   // \t   —— 水平制表符
#define ASCII_LF 0x0A                   // \n   —— 换行
#define ASCII_VT 0x0B                   // \v   —— 垂直制表符
#define ASCII_FF 0x0C                   // \f   —— 换页
#define ASCII_CR 0x0D                   // \r   —— 回车
#define ASCII_DEL 0x7F

static u32 screen;                      // 当前显示起始地址，
                                        // screen = MEM_BASE + (CRT_START_ADDR_H << 8 | CRT_START_ADDR_L) << 1

static u32 cursor;                      // 当前光标地址
                                        // cursor = MEM_BASE + (CRT_CURSOR_H << 8 | CRT_CURSOR_L) << 1

static u32 x, y;                        // 当前光标坐标
                                        // x = ((cursor - screen) >> 1) % WIDTH
                                        // y = ((cursor - screen) >> 1) / WIDTH


// 文本模式下，每个字符占 2 字节，低字节是字符的 ASCII 码，高字节是字符的属性
// 属性字节：位 7 —— 背景色是否闪烁；位 6 ~ 4 —— 背景色的 RGB；位 3 —— 前景色是否高亮；位 2 ~ 0 —— 前景色的 RGB
static u8 attr = 7;                     // 白底黑字
static u16 erase = 0x0720;              // 白底黑字的空格

// 获取当前显示起始地址
static void get_screen(void)
{   
    // 获取显示起始地址高 8 位
    outb(CRT_ADDR_REG, CRT_START_ADDR_H);
    screen = inb(CRT_DATA_REG) << 8;
    
    // 获取显示起始地址低 8 位
    outb(CRT_ADDR_REG, CRT_START_ADDR_L);
    screen |= inb(CRT_DATA_REG);

    screen <<= 1;
    screen += MEM_BASE;
}

// 设置当前显示起始地址
static void set_screen(void)
{   
    // 设置显示起始地址高 8 位
    outb(CRT_ADDR_REG, CRT_START_ADDR_H);
    outb(CRT_DATA_REG, ((screen - MEM_BASE) >> 9) & 0xff);
    
    // 获取显示起始地址低 8 位
    outb(CRT_ADDR_REG, CRT_START_ADDR_L);
    outb(CRT_DATA_REG, ((screen - MEM_BASE) >> 1) & 0xff);
}

// 获取当前光标地址
static void get_cursor(void)
{
    // 获取光标位置高 8 位
    outb(CRT_ADDR_REG, CRT_CURSOR_H);
    cursor = inb(CRT_DATA_REG) << 8;

    // 获取光标位置低 8 位
    outb(CRT_ADDR_REG, CRT_CURSOR_L);
    cursor |= inb(CRT_DATA_REG);

    get_screen();

    cursor <<= 1;
    cursor += MEM_BASE;

    x = ((cursor - screen) >> 1) % WIDTH;
    y = ((cursor - screen) >> 1) / WIDTH;
}

// 设置当前光标地址
static void set_cursor(void)
{
    // 设置光标位置高 8 位
    outb(CRT_ADDR_REG, CRT_CURSOR_H);
    outb(CRT_DATA_REG, ((cursor - MEM_BASE) >> 9) & 0xff);

    // 设置光标位置低 8 位
    outb(CRT_ADDR_REG, CRT_CURSOR_L);
    outb(CRT_DATA_REG, ((cursor - MEM_BASE) >> 1) & 0xff);
}

// 清空控制台
void console_clear(void)
{   
    screen = MEM_BASE;
    cursor = MEM_BASE;
    x = y = 0;
    set_cursor();
    set_screen();

    u16 *ptr = (u16 *)MEM_BASE;
    while (ptr < (u16 *)MEM_END)
    {
        *ptr++ = erase;
    }
}

static void command_bs(void)
{
    if (x > 0)
    {
        x--;
        cursor -= 2;
        *(u32 *)cursor = erase;
    }
}

// 向上滚动一行
static void scroll_up(void)
{
    // 如果后面没有多余的行了，则将显存内容复制到显存起始地址，重新开始滚动
    if (screen + SCR_SIZE + ROW_SIZE >= MEM_END)
    {
        memcpy((void *)MEM_BASE, (void *)screen, SCR_SIZE);
        cursor -= (screen - MEM_BASE);
        screen = MEM_BASE;
    }

    // 将要滚动出来的行清空
    u32 *ptr = (u32 *)(screen + SCR_SIZE);
    for (size_t i = 0; i < WIDTH; i++)
    {
        *ptr++ = erase;
    }

    // 向上滚动一行
    screen += ROW_SIZE;
    cursor += ROW_SIZE;
    set_screen();
}

static void command_lf(void)
{
    if (y + 1 < HEIGHT)
    {
        y++;
        cursor += ROW_SIZE;
    }
    else
    {
        scroll_up();
    }
}

static void command_cr(void)
{
    cursor -= (x << 1);
    x = 0;
}

static void command_del(void)
{
    *(u32 *)cursor = erase;
}

void start_beep(void);

// 向控制台写入数据
void console_write(const char *buf, u32 count)
{
    char ch;
    while (count--)
    {
        ch = *buf++;

        switch (ch)
        {
        case ASCII_NUL:
            break;
        case ASCII_ENQ:
            break;
        case ASCII_BEL:
            start_beep();
            break;
        case ASCII_BS:
            command_bs();
            break;
        case ASCII_HT:
            break;
        case ASCII_LF:
            command_cr();
            command_lf();
            break;
        case ASCII_VT:
            /* code */
            break;
        case ASCII_FF:
            /* code */
            break;
        case ASCII_CR:
            command_cr();
            break;
        case ASCII_DEL:
            command_del();
            break;
        default:
            if (x >= WIDTH)
            {
                x -= WIDTH;
                cursor -= ROW_SIZE;
                command_lf();
            }

            *((char *)cursor++) = ch;
            *((char *)cursor++) = attr;

            x++;
            break;
        }
    }
    set_cursor();
}

void console_init(void)
{
    console_clear();
}


