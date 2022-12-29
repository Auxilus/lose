/* This will force us to create a kernel entry function instead of jumping to kernel.c:0x00 */

#include "../drivers/vga.h"
#include "../drivers/pci.h"
#include "../drivers/acpi.h"
#include "../drivers/ports.h"
#include "../drivers/serial.h"
#include "../utils/mem.h"
#include "../utils/string.h"
#include "../cpu/isr.h"
#include "../cpu/timer.h"
#include "./graphics.h"
#include "kernel.h"

void dummy_test_entrypoint() {
}

void kernel_handle_key(key_event ke)
{
	if (ke.is_shift && ke.is_ctrl && ke.letter == 'Z') {
		// basic shutdown for qemu ctrl+shift+c
		serial_print("kernel: kernel shutdown initiated\n");
		gr_clear_screen();
		gr_print_string(10, 10, "shutting down...");
		timer_sleep(1);
		port_word_out(0x604, 0x2000);
		return;
	}

	if (ke.is_shift && ke.is_ctrl && ke.letter == 'C') {
		// basic shutdown for qemu ctrl+shift+c
		gr_clear_screen();
		windowctx->cursor_x = 0;
		windowctx->cursor_y = 0;
		return;
	}

	switch (ke.letter) {
		case '\n':
			gr_print_character(windowctx->cursor_x, windowctx->cursor_y, ' ');
			windowctx->cursor_x = 0;
			windowctx->cursor_y += 8;
			gr_print_character(windowctx->cursor_x, windowctx->cursor_y, '_');
			break;
		case '\t':
			gr_print(' ');
			gr_print(' ');
			break;
		case 0x08:
			gr_print_character(windowctx->cursor_x, windowctx->cursor_y, ' ');
			if (windowctx->cursor_x > 8) {
				windowctx->cursor_x -= 8;
			} else {
				windowctx->cursor_x = 0;
			}
			gr_print_character(windowctx->cursor_x, windowctx->cursor_y, '_');
			break;
		default:
			gr_print(ke.letter);
	}
}

void main() {
	serial_init();
	isr_install();
	irq_install();

	serial_print("KERNEL: testing interrupts\n");
	asm("int $2");
	asm("int $3");
	serial_print("KERNEL: interrupt test complete\n");
	
	serial_print("KERNEL: testing malloc\n");
	int *a = (int*)malloc(sizeof(int) * 5);
	char *s = (char*)malloc(64);

	char message1[128];
	sprintf(message1, "KERNEL: int[5]   0x%p\n", a);
	char message2[128];
	sprintf(message2, "KERNEL: char[64] 0x%p\n", s);

	serial_print(message1);
	serial_print(message2);
	acpi_init();
	pci_init();

	gr_init_graphics();


	gr_print_string(520, 430, "VGA mode 0x12\nfont test\n640x480x16");
	while(1);
}
