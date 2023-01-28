C_SOURCES = $(wildcard kernel/*.c kernel/shell/*.c drivers/*.c utils/*.c cpu/*.c)
ASM_SOURCES = ${wildcard cpu/*.asm}
HEADERS = $(wildcard kernel/*.h drivers/*.h utils/*.h cpu/*.h)
OBJ = ${C_SOURCES:.c=.o}
ASMOBJ = ${ASM_SOURCES:.asm=.o}

CC = i386-elf-gcc
GDB = i386-elf-gdb
CFLAGS = -g -fcommon
QEMU_OPTIONS = -d cpu_reset,guest_errors -no-reboot -serial stdio -rtc base=localtime \
							 -drive file=data/lose.img,index=0,media=disk,format=raw,if=ide

all: os-image.bin

os-image.bin: boot/boot.bin kernel.bin
	cat $^ > os-image.bin

kernel.bin: boot/kernel_entry.o ${OBJ} ${ASMOBJ}
	i386-elf-ld -o $@ -Ttext 0x1000 $^ --oformat binary

kernel.elf: boot/kernel_entry.o ${OBJ} ${ASMOBJ}
	i386-elf-ld -o $@ -Ttext 0x1000 $^ 

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

run: os-image.bin
	qemu-system-i386 ${QEMU_OPTIONS} -fda $<

run-bin: os-image.bin
	qemu-system-i386 ${QEMU_OPTIONS} -fda os-image.bin

debug: os-image.bin kernel.elf
	qemu-system-i386 -s -S ${QEMU_OPTIONS} -fda os-image.bin & #-fda os-image.bin &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -rf iso/  *.bin *.dis *.o os-image.bin *.elf *.iso *.img
	rm -rf **/*.o boot/*.bin drivers/*.o boot/*.o kernel/shell/*.o
