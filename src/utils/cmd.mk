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

QEMU_DISK := -drive file=$(BUILD)/master.img,if=ide,index=0,media=disk,format=raw # 主硬盘
QEMU_DISK += -drive file=$(BUILD)/slave.img,if=ide,index=1,media=disk,format=raw	# 从硬盘

QEMU+= -chardev stdio,mux=on,id=com1 # 字符设备 1，宿主机的标准输入输出
# QEMU+= -chardev vc,mux=on,id=com1 # 字符设备 1，QEMU 的虚拟控制台
# QEMU+= -chardev vc,mux=on,id=com2 # 字符设备 2，QEMU 的虚拟控制台
# QEMU+= -chardev udp,mux=on,id=com2,port=7777,ipv4=on # 字符设备 2，UDP 端口 7777
QEMU+= -serial chardev:com1 # 串口 1
# QEMU+= -serial chardev:com2 # 串口 2

QEMU_DISK_BOOT := -boot c	# 从第一块硬盘启动

QEMU_DEBUG := -s -S	# -s：指示 QEMU 在启动时开启一个 GDB 远程调试服务器，并监听本地的 TCP 端口 1234。-S：指示 QEMU 在启动时暂停执行，等待 GDB 连接。

.PHONY: qemu
qemu: $(IMAGES)
	$(QEMU) $(QEMU_DISK) $(QEMU_DISK_BOOT)

.PHONY: qemugdb
qemugdb: $(IMAGES)
	$(QEMU) $(QEMU_DISK) $(QEMU_DISK_BOOT) $(QEMU_DEBUG)

# VMWare 磁盘转换
$(BUILD)/master.vmdk: $(BUILD)/master.img
	qemu-img convert -O vmdk $< $@

.PHONY:vmdk
vmdk: $(BUILD)/master.vmdk