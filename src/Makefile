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
	$(BUILD)/kernel/ide.o \
	$(BUILD)/kernel/memory.o \
	$(BUILD)/kernel/arena.o \
	$(BUILD)/kernel/keyboard.o \
	$(BUILD)/kernel/buffer.o \
	$(BUILD)/lib/bitmap.o \
	$(BUILD)/lib/list.o \
	$(BUILD)/lib/fifo.o \
	$(BUILD)/lib/string.o \
	$(BUILD)/lib/vsprintf.o \
	$(BUILD)/lib/stdlib.o \
	$(BUILD)/lib/syscall.o \
	$(BUILD)/lib/printf.o \

	mkdir -p $(dir $@)
	ld ${LDFLAGS} $^ -o $@

# 转换为纯二进制内核文件 system.bin
$(BUILD)/system.bin: $(BUILD)/kernel.bin
	objcopy -O binary $< $@

# 提取符号表 system.map
$(BUILD)/system.map: $(BUILD)/kernel.bin
	nm $^ | sort > $@

# 创建一个 16MB 的硬盘镜像文件 master.img
$(BUILD)/master.img: \
	$(BUILD)/boot/mbr.bin \
	$(BUILD)/boot/loader.bin \
	$(BUILD)/system.bin \
	$(BUILD)/system.map	\
	$(SRC)/utils/master.sfdisk \

# 创建一个 16MB 的硬盘镜像
	yes | bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat $@
# 将 mbr.bin 写入硬盘
	dd if=$(BUILD)/boot/mbr.bin of=$@ bs=512 count=1 conv=notrunc
# 将 loader.bin 写入硬盘
	dd if=$(BUILD)/boot/loader.bin of=$@ bs=512 count=4 seek=2 conv=notrunc
# 将 system.bin 写入硬盘
	dd if=$(BUILD)/system.bin of=$@ bs=512 count=200 seek=10 conv=notrunc
# 对硬盘进行分区
	sfdisk $@ < $(SRC)/utils/master.sfdisk

# 创建一个 32MB 的硬盘镜像文件 slave.img
$(BUILD)/slave.img:
	yes | bximage -q -hd=32 -func=create -sectsize=512 -imgmode=flat $@

IMAGES := $(BUILD)/master.img $(BUILD)/slave.img

.PHONY: image
image: $(IMAGES)

.PHONY: clean
clean:
	rm -rf $(BUILD)

.PHONY: bochs
bochs: $(IMAGES)
	bochs -q -f ../bochs/bochsrc -unlock

QEMU := qemu-system-i386 # 虚拟机
QEMU += -m 32M # 内存
QEMU += -audiodev pa,id=hda # 音频设备
QEMU += -machine pcspk-audiodev=hda # PC Speaker 设备
QEMU += -rtc base=localtime # 设备本地时间
QEMU += -drive file=$(BUILD)/master.img,if=ide,index=0,media=disk,format=raw # 主硬盘
QEMU += -drive file=$(BUILD)/slave.img,if=ide,index=1,media=disk,format=raw	# 从硬盘

QEMU_DISK := -boot c	# 从第一块硬盘启动

QEMU_DEBUG := -s -S	# -s：指示 QEMU 在启动时开启一个 GDB 远程调试服务器，并监听本地的 TCP 端口 1234。-S：指示 QEMU 在启动时暂停执行，等待 GDB 连接。

.PHONY: qemu
qemu: $(IMAGES)
	$(QEMU) $(QEMU_DISK)

.PHONY: qemugdb
qemugdb: $(IMAGES)
	$(QEMU) $(QEMU_DISK) $(QEMU_DEBUG)

# VMWare 磁盘转换
$(BUILD)/master.vmdk: $(BUILD)/master.img
	qemu-img convert -O vmdk $< $@

.PHONY:vmdk
vmdk: $(BUILD)/master.vmdk