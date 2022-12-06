all:
	nasm -f bin bootloader/boot.asm -o bin/boot.bin
	nasm -f bin kernel/kernel.asm -o bin/kernel.bin
	cat bin/boot.bin bin/kernel.bin > bin/os.bin

c:
	nasm -f bin bootloader/boot.asm -o bin/boot.bin
	gcc kernel/kernel.c -o kernel/kernel.bin -std=gnu99 -nostdlib -nostartfiles -nodefaultlibs  
	objcopy -O binary kernel/kernel.bin bin/kernel.bin
	truncate -s 512 bin/kernel.bin
	cat bin/boot.bin bin/kernel.bin > bin/os.bin



run: all
	qemu-system-x86_64 -d cpu_reset,guest_errors -drive file=bin/os.bin,format=raw

debug: all
	qemu-system-x86_64 -s -S -d cpu_reset,guest_errors -drive file=bin/os.bin,format=raw

nographic: all
	qemu-system-x86_64 -d -fda bin/os.bin -nographic -curses

clean:
	rm **/*.bin
