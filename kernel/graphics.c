#include "graphics.h"
#include "font8x8_basic.h"
#include "../drivers/vga.h"
#include "../drivers/ports.h"

void gr_init_graphics(void)
{
	print_serial("    clearing framebuffer at address 0xA0000\n");
	gr_clear_screen();
}

void gr_clear_screen(void)
{
	for(u8 p = 3; p < 4; p--)
	{
		set_plane(p);
		memset(GR_START, BLACK, 64 * 1024);
	}
	return 0;
}

void gr_print_string(int x, int y, char* string)
{
	int cx = x;
	for (int i = 0; i < strlen(string); i++) {


		if (string[i] == 10 | cx > GR_WIDTH) {
			// new line or wrap, reset x and move cursor back to x
			y = y + 8;
			cx = x;
		}
		else if (string[i] == 13) {
			// \r, just reset the cursor
			cx = x;
		}
		else {
			int character = string[i];
			gr_print_character(cx, y, character);
			cx = cx+8;
		}
	}
}
void gr_print_character(int x, int y, int character)
{
	int cx, cy;
	int posx = x, posy = y;
	int set, mask;
	char *bitmap = font8x8_basic[character];

	for (cx=0; cx < 8; cx++) {
		for (cy=0; cy < 8; cy++) {
			set = bitmap[cx] & 1 << cy;
			vga_mode12h_pixel(set ? WHITE : BLACK, (u16)x+cy, (u16)y+cx);
		}
	}
}
