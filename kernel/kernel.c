/* This will force us to create a kernel entry function instead of jumping to kernel.c:0x00 */

#include "../drivers/vga.h"
#include "../drivers/ports.h"
#include "../drivers/serial.h"
#include "../utils/mem.h"
#include "../utils/string.h"
#include "../cpu/isr.h"
#include "./graphics.h"
#include "../drivers/keyboard.h"
#include "kernel.h"

void dummy_test_entrypoint() {
}

void kernel_handle_key(char letter)
{
	char *message;
	sprintf(message, "KERNEL: received keypress event: %c\n", letter);
	serial_print(message);
	gr_print(letter);
}

void main() {
	serial_init();
	isr_install();
	irq_install();
	gr_init_graphics();

	gr_draw_line(0, 0, 800, 500, WHITE);
	gr_draw_rect(100, 80, 80, 100, WHITE);

	serial_print("KERNEL: testing interrupts\n");
	asm("int $2");
	asm("int $3");
	serial_print("KERNEL: interrupt test complete\n");

	gr_print_string(520, 430, "VGA mode 0x12\nfont test\n640x480x16");
}
