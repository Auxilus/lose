/* This will force us to create a kernel entry function instead of jumping to kernel.c:0x00 */

#include "../drivers/vga.h"
#include "../drivers/ports.h"
#include "../drivers/serial.h"
#include "../utils/mem.h"
#include "../cpu/isr.h"
#include "./graphics.h"

void dummy_test_entrypoint() {
}


void main() {

	serial_init();

	serial_print("initializing interrupts\n");
	isr_install();
	irq_install();

	serial_print("initializing graphics\n");

	gr_init_graphics();

	gr_draw_line(0, 0, 800, 500, WHITE);
	gr_draw_rect(100, 80, 80, 100, WHITE);

	serial_print("testing interrupts\n");
	asm("int $2");
	asm("int $3");
	serial_print("interrupt test complete\n");
	gr_print_string(520, 430, "VGA mode 0x12\nfont test\n640x480x16");


	//int x = 0, y = 0;
	//while (1) {
	//	char ch = serial_read();
	//	gr_print_character(x, y, ch);
	//	x += 8;
	//	if (x>GR_WIDTH-8) {
	//		x = 0;
	//		y += 8;
	//	}
	//}
}
