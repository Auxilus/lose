#include "graphics.h"
#include "font8x8_basic.h"
#include "../drivers/vga.h"
#include "../drivers/ports.h"
#include "../drivers/serial.h"
#include "../utils/string.h"

int graphics_abs(int value) { return value < 0 ? 0 - value : value; }

void gr_init_graphics(void)
{
	serial_print("clearing framebuffer at address 0xA0000\n");
	gr_clear_screen();
}

void gr_clear_screen(void)
{
	for(u8 p = 3; p < 4; p--)
	{
		vga_set_plane(p);
		memset((u8*)GR_START, BLACK, 64 * 1024);
	}
}

void gr_print_string(int x, int y, char* string)
{
	int cx = x;
	for (int i = 0; i < strlen(string); i++) {


		if ((string[i] == 10) | (cx > GR_WIDTH)) {
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
	int set;
	char *bitmap = font8x8_basic[character];

	for (cx=0; cx < 8; cx++) {
		for (cy=0; cy < 8; cy++) {
			set = bitmap[cx] & 1 << cy;
			vga_mode12h_pixel(set ? WHITE : BLACK, (u16)x+cy, (u16)y+cx);
		}
	}
}

void gr_draw_line(int x0, int y0, int x1, int y1, char color)
{
	int dx = x1 - x0;
	int dy = y1 - y0;
	int xsign, ysign;

	if (dx > 0) {
		xsign = 1;
	}
	else {
		xsign = -1;
	}

	if (dy > 0) {
		ysign = 1;
	}
	else {
		ysign = -1;
	}

	dx = graphics_abs(dx);
	dy = graphics_abs(dy);

	int xx, xy, yx, yy;
	if (dx > dy) {
		xx = xsign;
		xy = 0;
		yx = 0;
		yy = ysign;
	}
	else {
		int tmp = dx;
		dx = dy;
		dy = tmp;
		xx = 0;
		xy = ysign;
		yx = xsign;
		yy = 0;
	}

	int D = 2*dy - dx;
	int y = 0;

	for (int x = 0; x < dx; x++) {
		int pixel_x = x0 + x*xx + y*yx;
		int pixel_y = y0 + x*xy + y*yy;
		if (pixel_x > 640) pixel_x = 640;
		if (pixel_x < 0) pixel_x = 0;
		if (pixel_y > 480) pixel_y = 480;
		if (pixel_y < 0) pixel_y = 0;

		vga_mode12h_pixel(color, pixel_x, pixel_y);

		if (D >= 0) {
			y += 1;
			D -= 2*dx;
		}
		D += 2*dy;
	}
}

void gr_draw_rect(int x0, int y0, int w, int h, char color)
{
	gr_draw_line(x0,		y0,		x0+w, y0,		color);
	gr_draw_line(x0,		y0+h, x0+w, y0+h,	color);
	gr_draw_line(x0+w,	y0,		x0+w, y0+h,	color);
	gr_draw_line(x0,		y0,		x0,		y0+h,	color);
}
