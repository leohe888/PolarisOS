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

# 内核入口地址
ENTRY_POINT := 0x10000

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

# 链接生成 ELF 格式的内核可执行文件 kernel.bin
$(BUILD)/kernel.bin: \
	$(BUILD)/kernel/start.o \
	$(BUILD)/kernel/main.o \
	$(BUILD)/kernel/io.o \
	$(BUILD)/kernel/console.o \
	$(BUILD)/kernel/printk.o \
	$(BUILD)/kernel/assert.o \
	$(BUILD)/kernel/debug.o \
	$(BUILD)/kernel/global.o \
	$(BUILD)/kernel/task.o \
	$(BUILD)/kernel/schedule.o \
	$(BUILD)/kernel/interrupt.o \
	$(BUILD)/kernel/handler.o \
	$(BUILD)/kernel/clock.o \
	$(BUILD)/kernel/time.o \
	$(BUILD)/kernel/rtc.o \
	$(BUILD)/lib/string.o \
	$(BUILD)/lib/vsprintf.o \
	$(BUILD)/lib/stdlib.o \

	mkdir -p $(dir $@)
	ld -m elf_i386 -static $^ -o $@ -Ttext $(ENTRY_POINT)

# 转换为纯二进制内核文件 system.bin
$(BUILD)/system.bin: $(BUILD)/kernel.bin
	objcopy -O binary $< $@

# 提取符号表 system.map
$(BUILD)/system.map: $(BUILD)/kernel.bin
	nm $^ | sort > $@

# 创建硬盘镜像文件 master.img
$(BUILD)/master.img: \
	$(BUILD)/boot/mbr.bin \
	$(BUILD)/boot/loader.bin \
	$(BUILD)/system.bin \
	$(BUILD)/system.map

	yes | bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat $@
	dd if=$(BUILD)/boot/mbr.bin of=$@ bs=512 count=1 conv=notrunc
	dd if=$(BUILD)/boot/loader.bin of=$@ bs=512 count=4 seek=2 conv=notrunc
	dd if=$(BUILD)/system.bin of=$@ bs=512 count=200 seek=10 conv=notrunc

.PHONY: test
test: $(BUILD)/master.img

.PHONY: clean
clean:
	rm -rf $(BUILD)

.PHONY: bochs
bochs: $(BUILD)/master.img
	bochs -q -f ../bochs/bochsrc

QEMU:= qemu-system-i386 \
	-m 32M \
	-boot c \
	-drive file=$(BUILD)/master.img,if=ide,index=0,media=disk,format=raw \
	-audiodev pa,id=hda \
	-machine pcspk-audiodev=hda \
	-rtc base=localtime \

.PHONY: qemu
qemu: $(BUILD)/master.img
	$(QEMU)

.PHONY: qemug
qemug: $(BUILD)/master.img
	$(QEMU) -s -S

$(BUILD)/master.vmdk: $(BUILD)/master.img
	qemu-img convert -O vmdk $< $@

.PHONY: vmdk
vmdk: $(BUILD)/master.vmdk
