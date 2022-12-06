all:
	nasm -f bin bootloader/boot.asm -o bin/boot.bin
	nasm -f bin kernel/kernel.asm -o bin/kernel.bin
	cat bin/boot.bin bin/kernel.bin > bin/os.bin

run: all
	qemu-system-x86_64 -fda bin/os.bin

nographic: all
	qemu-system-x86_64 -fda bin/os.bin -nographic -curses

clean:
	rm **/*.bin
