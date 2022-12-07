#include "graphics.h"
#include "font8x8_basic.h"
#include "../drivers/vga.h"

void gr_init_graphics(void)
{
	gr_clear_screen();
}

void gr_clear_screen(void)
{
	memset(GR_START, GR_COLOR_BG, GR_WIDTH*GR_HEIGHT);
}

void gr_print_string(int x, int y, char* string, int length)
{
	for (int i = 0; i < length; i++) {
		int character = string[i];
		gr_print_character(x+(8*i), y, character);
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
			vga_set_pixel(x+cy, y+cx, set ? 4 : GR_COLOR_BG);
		}
	}
}
