C_SOURCES = $(wildcard kernel/*.c kernel/shell/*.c kernel/fs/*.c drivers/*.c utils/*.c cpu/*.c)
ASM_SOURCES = ${wildcard cpu/*.asm}
HEADERS = $(wildcard kernel/*.h kernel/fs/*.h kernel/shell/*.h drivers/*.h utils/*.h cpu/*.h)
OBJ = ${C_SOURCES:.c=.o}
ASMOBJ = ${ASM_SOURCES:.asm=.o}

CC = i386-elf-gcc
GDB = i386-elf-gdb
CFLAGS = -g -fcommon
QEMU_OPTIONS = -d cpu_reset,guest_errors -no-reboot -serial stdio -rtc base=localtime \
							 -drive file=lose.img,index=0,media=disk,format=raw,if=ide \
							 -boot d

all: os-image.bin lose.img

os-image.bin: boot/boot.bin kernel.bin
	cat boot/boot.bin kernel.bin > os-image.bin

kernel.bin: boot/kernel_entry.o ${OBJ} ${ASMOBJ}
	i386-elf-ld -o $@ -Ttext 0x1000 $^ --oformat binary

kernel.elf: boot/kernel_entry.o ${OBJ} ${ASMOBJ}
	i386-elf-ld -o $@ -Ttext 0x1000 $^ 

lose.img: boot/boot.bin kernel.bin
	echo "Copy files to lose.img"
	dd if=/dev/zero bs=512 count=2880 of=lose.img
	mkfs.msdos -n "LOSE DATA" -F 12 lose.img
	mmd -i lose.img "::DATA"
	mcopy -i lose.img boot/boot.bin "::BOOT.BIN"
	mcopy -i lose.img kernel.bin "::KERNEL.BIN"
	mcopy -i lose.img data/test.txt "::DATA/TEXT.TXT"

lose.iso: os-image.bin
	dd if=/dev/zero of=floppy.img bs=1024 count=1440
	dd if=$< of=floppy.img seek=0 conv=notrunc
	mkdir -p iso
	cp floppy.img iso/
	echo "ISO TEST FILE" >> iso/test.txt
	genisoimage -quiet -V 'LOSE' \
		-input-charset iso8859-1 \
		-o lose.iso \
		-b floppy.img \
		-hide floppy.img iso/

ng: os-image.bin  lose.img
	qemu-system-i386 ${QEMU_OPTIONS} -curses -fda $<

run: os-image.bin  lose.img
	qemu-system-i386 ${QEMU_OPTIONS} -fda $<

debug: os-image.bin kernel.elf  lose.img
	qemu-system-i386 -s -S ${QEMU_OPTIONS} -fda os-image.bin &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -rf iso/  *.bin *.dis *.o os-image.bin *.elf *.iso *.img
	rm -rf **/*.o boot/*.bin drivers/*.o boot/*.o kernel/shell/*.o kernel/fs/*.o
