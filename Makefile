all:
	nasm -f bin bootloader/boot.asm -o bin/boot.bin
	nasm -f bin kernel/kernel.asm -o bin/kernel.bin
	cat bin/boot.bin bin/kernel.bin > bin/os.bin

c:
	nasm -f bin bootloader/boot.asm -o bin/boot.bin
	gcc kernel/kernel.c -o kernel/kernelc.o -std=gnu99 -ffreestanding -nostdlib -nostartfiles -nodefaultlibs  
	ld -o kernel/kernel.bin -Ttext 0x0000 kernel/kernelc.o
	objcopy -O binary kernel/kernelc.bin bin/kernelc.bin
	truncate -s 512 bin/kernelc.bin
	cat bin/boot.bin bin/kernelc.bin > bin/osc.bin

run: all
	qemu-system-x86_64 -d cpu_reset,guest_errors -drive file=bin/os.bin,format=raw

runc: c
	qemu-system-x86_64 -d cpu_reset,guest_errors -drive file=bin/osc.bin,format=raw

debug: all
	qemu-system-x86_64 -s -S -d cpu_reset,guest_errors -drive file=bin/os.bin,format=raw

nographic: all
	qemu-system-x86_64 -d -fda bin/os.bin -nographic -curses

clean:
	rm **/*.bin
