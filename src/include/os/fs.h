#ifndef OS_FS_H
#define OS_FS_H

/*
Minix 1 文件系统将存储设备划分为 6 个连续部分：

- 引导块（Boot Block）：位于第 0 块，存放引导程序，用于引导操作系统。
- 超级块（Super Block）：第 1 块，存储文件系统的全局元数据。
- inode 位图：位于超级块后。用于标记每个 inode 是否空闲。第 0 位保留不用（恒为 1）。第 1 位是根目录的 inode。
- 数据块位图：位于 inode 位图后。用于标记每个数据块是否空闲。第 0 位保留不用（恒为 1）。第 1 位是根目录的数据块。
- inode 区：位于数据块位图后，存放所有 inode 的数组，每个文件/目录对应一个唯一的 inode 。inode 编号 - 1 才是 inode 数组中的索引。
- 数据区：位于 inode 表后，存放实际的文件数据。
*/

#include <os/types.h>
#include <os/list.h>

#define BLOCK_SIZE 1024 // 块大小
#define SECTOR_SIZE 512 // 扇区大小

#define MINIX1_MAGIC 0x137F // 文件系统魔数
#define NAME_LEN 14         // 文件名长度

#define IMAP_NR 8 // inode 位图占用的块数的最大值
#define ZMAP_NR 8 // 数据块位图占用的块数的最大值

#define BLOCK_BITS (BLOCK_SIZE * 8)                      // 块位图大小
#define BLOCK_INODES (BLOCK_SIZE / sizeof(inode_desc_t)) // 块 inode 数量   1024 / 16 = 64
#define BLOCK_DENTRIES (BLOCK_SIZE / sizeof(dentry_t))   // 块 dentry 数量  1024 / 16 = 64
#define BLOCK_INDEXES (BLOCK_SIZE / sizeof(u16))         // 块索引数量      1024 / 2 = 512

#define DIRECT_BLOCK 7                                                 // 直接块数量        7
#define INDIRECT1_BLOCK BLOCK_INDEXES                                  // 一级间接块数量    512
#define INDIRECT2_BLOCK (INDIRECT1_BLOCK * INDIRECT1_BLOCK)            // 二级间接块数量    512 * 512 = 262144
#define TOTAL_BLOCK (DIRECT_BLOCK + INDIRECT1_BLOCK + INDIRECT2_BLOCK) // 全部块数量        7 + 512 + 262144 = 262663

#define SEPARATOR1 '/'                                       // 目录分隔符 1
#define SEPARATOR2 '\\'                                      // 目录分隔符 2
#define IS_SEPARATOR(c) (c == SEPARATOR1 || c == SEPARATOR2) // 字符是否位目录分隔符

enum file_flag
{
    O_RDONLY = 00,      // 只读方式
    O_WRONLY = 01,      // 只写方式
    O_RDWR = 02,        // 读写方式
    O_ACCMODE = 03,     // 文件访问模式屏蔽码
    O_CREAT = 00100,    // 如果文件不存在就创建
    O_EXCL = 00200,     // 独占使用文件标志
    O_NOCTTY = 00400,   // 不分配控制终端
    O_TRUNC = 01000,    // 若文件已存在且是写操作，则长度截为 0
    O_APPEND = 02000,   // 以添加方式打开，文件指针置为文件尾
    O_NONBLOCK = 04000, // 非阻塞方式打开和操作文件
};

typedef enum whence_t
{
    SEEK_SET = 1, // 直接设置偏移
    SEEK_CUR,     // 当前位置偏移
    SEEK_END      // 结束位置偏移
} whence_t;

// 磁盘上的 inode
typedef struct inode_desc_t
{
    u16 mode;    // 文件类型与权限
    u16 uid;     // 用户 ID
    u32 size;    // 文件大小（字节）
    u32 mtime;   // 修改时间戳，这个时间戳应该用 UTC 时间，不然有瑕疵
    u8 gid;      // 组 ID
    u8 nlinks;   // 硬链接数（有多少个文件目录项指向该 inode ）
    u16 zone[9]; // 直接 (0-6)、间接(7)或双重间接 (8) 逻辑块号
} inode_desc_t;

// 内存中的 inode
typedef struct inode_t
{
    inode_desc_t *desc;   // inode 描述符
    struct buffer_t *buf; // inode 描述符对应的缓冲区
    dev_t dev;            // 设备号
    idx_t nr;             // inode 号
    u32 count;            // 引用计数
    time_t atime;         // 访问时间
    time_t ctime;         // 修改时间
    list_node_t node;     // 链表结点
    dev_t mount;          // 安装设备
} inode_t;

// 磁盘上的超级快
typedef struct super_desc_t
{
    u16 inodes;        // 总 inode 数
    u16 zones;         // 总块数
    u16 imap_blocks;   // inode 位图占用的块数
    u16 zmap_blocks;   // 数据块位图占用的块数
    u16 firstdatazone; // 第一个数据块的块号
    u16 log_zone_size; // 数据块大小（对数表示），即 2^log_zone_size KB
    u32 max_size;      // 文件最大长度
    u16 magic;         // 文件系统魔数
} super_desc_t;

// 内存中的超级块
typedef struct super_block_t
{
    super_desc_t *desc;              // 超级块描述符
    struct buffer_t *buf;            // 超级块描述符缓冲区指针
    struct buffer_t *imaps[IMAP_NR]; // inode 位图缓冲区指针数组
    struct buffer_t *zmaps[ZMAP_NR]; // 数据块位图缓冲区指针数组
    dev_t dev;                       // 设备号
    u32 count;                       // 引用计数
    list_t inode_list;               // 使用中 inode 链表
    inode_t *iroot;                  // 根目录 inode
    inode_t *imount;                 // 安装到的 inode
} super_block_t;

// 文件目录项结构
typedef struct dentry_t
{
    u16 nr;              // inode 节点号
    char name[NAME_LEN]; // 文件名
} dentry_t;

typedef struct file_t
{
    inode_t *inode; // 文件 inode
    u32 count;      // 引用计数
    off_t offset;   // 文件偏移
    int flags;      // 文件标记
    int mode;       // 文件模式
} file_t;

typedef dentry_t dirent_t;

super_block_t *get_super(dev_t dev);  // 获得 dev 对应的超级块
super_block_t *read_super(dev_t dev); // 读取 dev 对应的超级块

idx_t balloc(dev_t dev);          // 分配一个文件块
void bfree(dev_t dev, idx_t idx); // 释放一个文件块
idx_t ialloc(dev_t dev);          // 分配一个文件系统 inode
void ifree(dev_t dev, idx_t idx); // 释放一个文件系统 inode

// 获取 inode 第 block 块的索引值
// 如果不存在 且 create 为 true，则创建
idx_t bmap(inode_t *inode, idx_t block, bool create);

inode_t *get_root_inode();               // 获取根目录 inode
inode_t *iget(dev_t dev, idx_t nr);      // 获得设备 dev 的 nr inode
void iput(inode_t *inode);               // 释放 inode
inode_t *new_inode(dev_t dev, idx_t nr); // 创建新 inode

inode_t *named(char *pathname, char **next); // 获取 pathname 对应的父目录 inode
inode_t *namei(char *pathname);              // 获取 pathname 对应的 inode

// 打开文件，返回 inode
inode_t *inode_open(char *pathname, int flag, int mode);

// 从 inode 的 offset 处，读 len 个字节到 buf
int inode_read(inode_t *inode, char *buf, u32 len, off_t offset);

// 从 inode 的 offset 处，将 buf 的 len 个字节写入磁盘
int inode_write(inode_t *inode, char *buf, u32 len, off_t offset);

// 释放 inode 所有文件块
void inode_truncate(inode_t *inode);

file_t *get_file();
void put_file(file_t *file);

// 格式化文件系统
int devmkfs(dev_t dev, u32 icount);

#endif
