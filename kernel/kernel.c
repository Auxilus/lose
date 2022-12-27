/* This will force us to create a kernel entry function instead of jumping to kernel.c:0x00 */

#include "../drivers/vga.h"
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
		timer_sleep(2);
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
			windowctx->cursor_x = 0;
			windowctx->cursor_y += 8;
			break;
		case 0x08:
			if (windowctx->cursor_x > 8) {
				windowctx->cursor_x -= 8;
			} else {
				windowctx->cursor_x = 0;
			}
			gr_print_character(windowctx->cursor_x, windowctx->cursor_y, ' ');
			break;
		default:
			gr_print(ke.letter);
	}
}

void main() {
	serial_init();
	isr_install();
	irq_install();
	gr_init_graphics();

	//gr_draw_line(0, 0, 800, 500, WHITE);
	//gr_draw_rect(100, 80, 80, 100, WHITE);

	serial_print("KERNEL: testing interrupts\n");
	asm("int $2");
	asm("int $3");
	serial_print("KERNEL: interrupt test complete\n");

	gr_print_string(520, 430, "VGA mode 0x12\nfont test\n640x480x16");
}
