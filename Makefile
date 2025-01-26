CC =  gcc
LD = ld.lld
OBJCOPY = llvm-objcopy

#CFLAGS = -ffreestanding -nostdlib -mgeneral-regs-only -I.
CFLAGS = -g -target aarch64-unknown-none-elf \
         -Wall -Wextra \
         -ffreestanding -nostdlib \
         -I./drivers -I./lib -I./fs \
         -mno-outline-atomics -march=armv8-a
#LCL_FLAGS = -nostdlib -ffreestanding -Wl,--oformat=binary -Wl,-Ttext=0x8000

#lcl.bin: cli/lcl.c
#	gcc $(LCL_FLAGS) -o $@ $^
# Add CLI build rules
CLI_CC = gcc
CLI_CFLAGS = -ffreestanding -nostdlib -I./drivers -I./lib
all: kernel8.img

lcl: cli/lcl.c drivers/uart.c lib/string.c
	$(CLI_CC) $(CLI_CFLAGS) -c drivers/uart.c -o drivers/uart.o
	$(CLI_CC) $(CLI_CFLAGS) -c lib/string.c -o lib/string.o
	$(CLI_CC) $(CLI_CFLAGS) -c cli/lcl.c -o cli/lcl.o
	$(CLI_CC) -nostdlib -Wl,--entry=lcl_main -Wl,-Ttext=0x8000 \
		-o cli/lcl cli/lcl.o drivers/uart.o lib/string.o
#all: kernel8.img

kernel8.elf: boot.o kernel.o mm/mem.o drivers/uart.o drivers/ahci.o fs/ext2.o fs/vfs.o lib/string.o cli/lcl.o
	$(LD) -T linker.ld -nostdlib -o $@ $^

kernel8.img: kernel8.elf
	$(OBJCOPY) -O binary $< $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

boot.o: boot.S
	$(CC) $(CFLAGS) -c $< -o $@

mmu.o: mmu.S
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f *.o *.elf drivers/*.o fs/*.o lib/*.o cli/*.o

run: kernel8.img
#	qemu-system-aarch64 -M virt -cpu cortex-a53 -nographic -kernel kernel8.img
	qemu-system-aarch64 -M virt -cpu cortex-a53 -m 512M \
	-drive file=disk1.img,format=raw,id=disk,if=none \
	-device ahci,id=ahci \
	-device ide-hd,drive=disk,bus=ahci.0 \
	-nographic -kernel kernel8.img
runDebug: kernel8.img
	qemu-system-aarch64 -M virt -cpu cortex-a53 -m 512M \
        -drive file=disk1.img,format=raw,id=disk,if=none \
        -device ahci,id=ahci \
        -device ide-hd,drive=disk,bus=ahci.0 \
        -nographic -kernel kernel8.img -s -S
