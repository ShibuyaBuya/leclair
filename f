CC = clang
LD = ld.lld
OBJCOPY = llvm-objcopy

CFLAGS = -target aarch64-unknown-none-elf \
         -Wall -Wextra \
         -ffreestanding -nostdlib \
         -mno-outline-atomics

all: kernel8.img

kernel8.elf: boot.o kernel.o linker.ld
	$(LD) -T linker.ld -nostdlib -o $@ boot.o kernel.o

kernel8.img: kernel8.elf
	$(OBJCOPY) -O binary $< $@

boot.o: boot.S
	$(CC) $(CFLAGS) -c $< -o $@

kernel.o: kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.elf *.img

run: kernel8.img
	qemu-system-aarch64 -M virt -cpu cortex-a53 -nographic \
	-kernel kernel8.img -serial mon:stdio
