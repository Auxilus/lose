C_SOURCES = $(wildcard kernel/*.c drivers/*.c utils/*.c cpu/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h utils/*.h cpu/*.h)
# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o}

# Change this if your cross-compiler is somewhere else
CC = i386-elf-gcc
GDB = i386-elf-gdb
# -g: Use debugging symbols in gcc
CFLAGS = -g -Wall -fcommon

# First rule is run by default
os-image.bin: boot/boot.bin kernel.bin
	cat $^ > os-image.bin

# '--oformat binary' deletes all symbols as a collateral, so we don't need
# to 'strip' them manually on this case
kernel.bin: boot/kernel_entry.o ${OBJ} cpu/interrupt.o
	i386-elf-ld -o $@ -Ttext 0x1000 $^ --oformat binary

# Used for debugging purposes
kernel.elf: boot/kernel_entry.o ${OBJ}
	i386-elf-ld -o $@ -Ttext 0x1000 $^ 

run: os-image.bin
	qemu-system-i386 -d cpu_reset,guest_errors -serial stdio -fda os-image.bin

ng: os-image.bin
	qemu-system-i386 -curses -nographic -d guest_errors -fda os-image.bin
# Open the connection to qemu and load our kernel-object file with symbols
debug: os-image.bin kernel.elf
	qemu-system-i386 -s -S -fda os-image.bin &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

# Generic rules for wildcards
# To make an object, always compile from its .c
%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf
	rm -rf **/*.o boot/*.bin drivers/*.o boot/*.o
