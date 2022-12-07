/* This will force us to create a kernel entry function instead of jumping to kernel.c:0x00 */

#include "../drivers/vga.h"
#include "../drivers/ports.h"
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

		gr_init_graphics();
		//port_byte_out(0x3ce, 0x08);
		//port_byte_out(0x3cf, 0xff);
		//port_byte_out(0x3ce, 0x05);
		//port_byte_out(0x3cf, 0x02);

		gr_print_string(15, 15, "auxilus\nfont test");
}
