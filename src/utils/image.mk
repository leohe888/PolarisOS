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

# 挂载设备
	sudo losetup /dev/loop4 --partscan $@

# 创建 minux 文件系统（-1：第一版；-n 14：文件名最大长度为 14，/dev/loop4p1：loop4 设备的第一个分区）
	sudo mkfs.minix -1 -n 14 /dev/loop4p1

# 挂载文件系统
	sudo mount /dev/loop4p1 /mnt

# 切换所有者
	sudo chown ${USER} /mnt 

# 创建目录
	mkdir -p /mnt/dev
	mkdir -p /mnt/mnt

# 创建文件
	echo "hello PolarisOS!!!, from root direcotry file..." > /mnt/hello.txt

# 卸载文件系统
	sudo umount /mnt

# 卸载设备
	sudo losetup -d /dev/loop4

# 创建一个 32MB 的硬盘镜像文件 slave.img
$(BUILD)/slave.img:

# 创建一个 32MB 的硬盘镜像
	yes | bximage -q -hd=32 -func=create -sectsize=512 -imgmode=flat $@

# 执行硬盘分区
	sfdisk $@ < $(SRC)/utils/slave.sfdisk

# 挂载设备
	sudo losetup /dev/loop4 --partscan $@

# 创建 minux 文件系统
	sudo mkfs.minix -1 -n 14 /dev/loop4p1

# 挂载文件系统
	sudo mount /dev/loop4p1 /mnt

# 切换所有者
	sudo chown ${USER} /mnt 

# 创建文件
	echo "slave root direcotry file..." > /mnt/hello.txt

# 卸载文件系统
	sudo umount /mnt

# 卸载设备
	sudo losetup -d /dev/loop4

.PHONY: mount0
mount0: $(BUILD)/master.img
	sudo losetup /dev/loop4 --partscan $<
	sudo mount /dev/loop4p1 /mnt
	sudo chown ${USER} /mnt 

.PHONY: umount0
unmount0: /dev/loop4
	-sudo umount /mnt
	-sudo losetup -d $<

.PHONY: mount1
mount1: $(BUILD)/slave.img
	sudo losetup /dev/loop4 --partscan $<
	sudo mount /dev/loop4p1 /mnt
	sudo chown ${USER} /mnt 

.PHONY: umount1
unmount1: /dev/loop4
	-sudo umount /mnt
	-sudo losetup -d $<

IMAGES:= $(BUILD)/master.img \
	$(BUILD)/slave.img

image: $(IMAGES)