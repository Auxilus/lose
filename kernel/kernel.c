/* This will force us to create a kernel entry function instead of jumping to kernel.c:0x00 */

#include "../drivers/vga.h"
#include "../drivers/ports.h"
#include "../drivers/serial.h"
#include "../utils/mem.h"
#include "./graphics.h"

void dummy_test_entrypoint() {
}


void main() {
    //char* video_memory = (char*) 0xa0000; // VGA,MCGA 320x200 256 colors

		//while(video_memory++ < 0xa1010) {
		//	*video_memory = 'X';
		//}

		//for (int x = 0; x < 320; x++) {
		//	for (int y = 0; y < 200; y++) {
		//		vga_set_pixel(x, y, 0x11);
		//	}
		//}

		serial_init();

		serial_print("\n\n");
		serial_print("-------------------------------------\n");
		serial_print("Initializing graphics\n");

		gr_init_graphics();
		gr_draw_line(0, 0, 200, 200, WHITE);

		gr_print_string(520, 430, "VGA mode 0x12\nfont test\n640x480x16");

		int x = 0, y = 0;
		while (1) {
			char ch = serial_read();
			gr_print_character(x, y, ch);
			x += 8;
			if (x>GR_WIDTH-8) {
				x = 0;
				y += 8;
			}
		}
}
