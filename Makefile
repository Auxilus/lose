all:
	nasm -f bin bootloader/boot.asm -o boot.bin
	nasm -f bin kernel/kernel.asm -o kernel.bin
	cat boot.bin kernel.bin > bin/os.bin

run: all
	qemu-system-x86_64 -fda bin/os.bin

nographic: all
	qemu-system-x86_64 -fda bin/os.bin -nographic -curses

clean:
	rm **/*.bin
