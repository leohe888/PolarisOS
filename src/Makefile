# Makefile 赋值运算符：
# := —— 立即赋值，赋值时立即展开
# += —— 追加赋值，往原有变量追加值，中间用空格分隔
# = —— 延迟赋值，使用时递归展开
# ?= —— 条件赋值，仅在变量未定义时赋值

# Makefile 自动变量：
# $@：目标文件名
# $<：第一个依赖文件名
# $^：所有依赖文件名，以空格分隔
# $?：所有比目标文件新的依赖文件名，以空格分隔

BUILD 		:= ../build
SRC 		:= .

# multiboot2 头部起始地址
MULTIBOOT2:=0x10000

# 内核入口地址
ENTRYPOINT	:= $(shell printf "0x%x" $$(( $(MULTIBOOT2) + 64 )) )

CFLAGS		:= -m32					# 生成 32 位代码
CFLAGS		+= -fno-builtin			# 不使用 GCC 内置函数
CFLAGS		+= -nostdinc			# 不使用标准头文件
CFLAGS		+= -nostdlib			# 不使用标准库
CFLAGS		+= -fno-pic				# 不生成位置无关代码
CFLAGS		+= -fno-pie				# 不生成位置无关可执行文件
CFLAGS		+= -fno-stack-protector	# 不使用栈保护
CFLAGS		+= -DPolarisOS			# 定义 PolarisOS
CFLAGS		:= $(strip ${CFLAGS})

DEBUG 	:= -g

INCLUDE := -I$(SRC)/include

$(BUILD)/boot/%.bin: $(SRC)/boot/%.asm
	mkdir -p $(dir $@)
	nasm -f bin $< -o $@

$(BUILD)/%.o: $(SRC)/%.asm
	mkdir -p $(dir $@)
	nasm -f elf32 $(DEBUG) $< -o $@

$(BUILD)/%.o: $(SRC)/%.c
	mkdir -p $(dir $@)
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

BUILTIN_APPS := \
	$(BUILD)/builtin/init.out \
	$(BUILD)/builtin/posh.out \
	$(BUILD)/builtin/env.out \
	$(BUILD)/builtin/echo.out \
	$(BUILD)/builtin/cat.out \
	$(BUILD)/builtin/ls.out \
	$(BUILD)/builtin/dup.out \
	$(BUILD)/builtin/err.out \

$(BUILD)/lib/libc.o: $(BUILD)/lib/crt.o \
	$(BUILD)/lib/crt1.o \
	$(BUILD)/lib/string.o \
	$(BUILD)/lib/vsprintf.o \
	$(BUILD)/lib/stdlib.o \
	$(BUILD)/lib/syscall.o \
	$(BUILD)/lib/printf.o \
	$(BUILD)/lib/assert.o \
	$(BUILD)/lib/time.o \

	ld -m elf_i386 -r $^ -o $@

$(BUILD)/builtin/%.out: $(BUILD)/builtin/%.o \
	$(BUILD)/lib/libc.o \
	
	ld -m elf_i386 -static $^ -o $@ -Ttext 0x1001000

LDFLAGS:= -m elf_i386 \
		-static \
		-Ttext $(ENTRYPOINT)\
		--section-start=.multiboot2=$(MULTIBOOT2)
LDFLAGS:=$(strip ${LDFLAGS})

# 链接生成 ELF 格式的内核可执行文件 kernel.bin
$(BUILD)/kernel.bin: \
	$(BUILD)/kernel/start.o \
	$(BUILD)/kernel/main.o \
	$(BUILD)/kernel/io.o \
	$(BUILD)/kernel/device.o \
	$(BUILD)/kernel/console.o \
	$(BUILD)/kernel/printk.o \
	$(BUILD)/kernel/assert.o \
	$(BUILD)/kernel/debug.o \
	$(BUILD)/kernel/global.o \
	$(BUILD)/kernel/task.o \
	$(BUILD)/kernel/thread.o \
	$(BUILD)/kernel/mutex.o \
	$(BUILD)/kernel/gate.o \
	$(BUILD)/kernel/schedule.o \
	$(BUILD)/kernel/interrupt.o \
	$(BUILD)/kernel/handler.o \
	$(BUILD)/kernel/clock.o \
	$(BUILD)/kernel/time.o \
	$(BUILD)/kernel/rtc.o \
	$(BUILD)/kernel/ramdisk.o \
	$(BUILD)/kernel/ide.o \
	$(BUILD)/kernel/serial.o \
	$(BUILD)/kernel/memory.o \
	$(BUILD)/kernel/arena.o \
	$(BUILD)/kernel/keyboard.o \
	$(BUILD)/kernel/buffer.o \
	$(BUILD)/kernel/system.o \
	$(BUILD)/kernel/execve.o \
	$(BUILD)/fs/super.o \
	$(BUILD)/fs/bmap.o \
	$(BUILD)/fs/inode.o \
	$(BUILD)/fs/namei.o \
	$(BUILD)/fs/file.o \
	$(BUILD)/fs/stat.o \
	$(BUILD)/fs/dev.o \
	$(BUILD)/fs/pipe.o \
	$(BUILD)/lib/bitmap.o \
	$(BUILD)/lib/list.o \
	$(BUILD)/lib/fifo.o \
	$(BUILD)/lib/string.o \
	$(BUILD)/lib/vsprintf.o \
	$(BUILD)/lib/stdlib.o \
	$(BUILD)/lib/syscall.o \
	$(BUILD)/lib/printf.o \
	$(BUILD)/builtin/posh.o \

	mkdir -p $(dir $@)
	ld ${LDFLAGS} $^ -o $@

# 转换为纯二进制内核文件 system.bin
$(BUILD)/system.bin: $(BUILD)/kernel.bin
	objcopy -O binary $< $@

# 提取符号表 system.map
$(BUILD)/system.map: $(BUILD)/kernel.bin
	nm $^ | sort > $@

include utils/image.mk
include utils/cdrom.mk
include utils/cmd.mk