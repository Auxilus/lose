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

		init_serial();

		print_serial("\n\n");
		print_serial("---------- kernel ----------\n");
		print_serial("\nInitializing graphics\n");

		gr_init_graphics();

		for (u16 i = 0; i < 100; i++)
			vga_mode12h_pixel(RED, i, i);

		for (u16 i = 100; i > 0; i--)
			vga_mode12h_pixel(BLUE, i, 100-i);

		for (u16 i = 0; i < 100; i++)
			vga_mode12h_pixel(GREEN, i, 50);

		while(1){}


		//gr_print_string(5, 5, "auxilus\nfont test");
		//for(int i=0; i<128; i++) {
		//	gr_print_character(i*8, 30, i);
		//}
}
