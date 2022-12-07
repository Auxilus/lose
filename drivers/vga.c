#include "vga.h"

void vga_set_pixel(int pos_x, int pos_y, u8 VGA_COLOR)
{
	u8* location = (u8*)0xA0000 + 320 * pos_y + pos_x;
	*location = VGA_COLOR;
}
