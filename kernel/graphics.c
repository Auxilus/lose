#include "graphics.h"
#include "font8x8_basic.h"
#include "../drivers/vga.h"
#include "../drivers/ports.h"
#include "../drivers/serial.h"
#include "../utils/string.h"

int graphics_abs(int value) { return value < 0 ? 0 - value : value; }

void gr_init_graphics(void)
{
	serial_print("GRAPHICS: init graphics\n");
	gr_clear_screen();
	serial_print("GRAPHICS: init window context\n");
	windowctx->cursor_x = 0;
	windowctx->cursor_y = 0;
	windowctx->charbuf = (char *)malloc((GR_HEIGHT / 8) * (GR_WIDTH / 8));
	memset(windowctx->charbuf, 0, (GR_HEIGHT / 8) * (GR_WIDTH / 8));
	gr_print_character(0, 0, '_');
	serial_print("GRAPHICSL: init done\n");
}

void gr_clear_screen(void)
{
	serial_print("GRAPHICS: clearing plane framebuffer\n");

	char s[1];
	for (u8 p = 3; p < 4; p--)
	{
		char str[24];
		sprintf(str, "GRAPHICS: plane[%u]\n", p);
		serial_print(str);
		vga_set_plane(p);
		memset((u8 *)GR_START, BLACK, 64 * 1024);
	}
}

void gr_print_string(int x, int y, char *string)
{
	int cx = x;
	for (int i = 0; i < strlen(string); i++)
	{

		if ((string[i] == 10) | (cx > GR_WIDTH))
		{
			// new line or wrap, reset x and move cursor back to x
			y = y + 8;
			cx = x;
		}
		else if (string[i] == 13)
		{
			// \r, just reset the cursor
			cx = x;
		}
		else
		{
			int character = string[i];
			gr_print_character(cx, y, character);
			cx = cx + 8;
		}
	}
}

void gr_print_character(int x, int y, int character)
{
	int cx, cy;
	int set;
	char *bitmap = font8x8_basic[character];

	// char msg[64];
	// sprintf(msg, "x %d y %d buffer idx %u\n", x, y, gr_window_get_buffer_idx(x, y));
	// serial_print(msg);
	if ((windowctx->cursor_y + 8) > GR_HEIGHT - 8)
	{
		gr_window_scroll();
	}

	int winbuf_idx = gr_window_get_buffer_idx(x, y);
	windowctx->charbuf[winbuf_idx] = (char)character;

	for (cx = 0; cx < 8; cx++)
	{
		for (cy = 0; cy < 8; cy++)
		{
			set = bitmap[cx] & 1 << cy;
			vga_mode12h_pixel(set ? WHITE : BLACK, (u16)x + cy, (u16)y + cx);
		}
	}
}

void gr_draw_line(int x0, int y0, int x1, int y1, char color)
{
	int dx = x1 - x0;
	int dy = y1 - y0;
	int xsign, ysign;

	if (dx > 0)
	{
		xsign = 1;
	}
	else
	{
		xsign = -1;
	}

	if (dy > 0)
	{
		ysign = 1;
	}
	else
	{
		ysign = -1;
	}

	dx = graphics_abs(dx);
	dy = graphics_abs(dy);

	int xx, xy, yx, yy;
	if (dx > dy)
	{
		xx = xsign;
		xy = 0;
		yx = 0;
		yy = ysign;
	}
	else
	{
		int tmp = dx;
		dx = dy;
		dy = tmp;
		xx = 0;
		xy = ysign;
		yx = xsign;
		yy = 0;
	}

	int D = 2 * dy - dx;
	int y = 0;

	for (int x = 0; x < dx; x++)
	{
		int pixel_x = x0 + x * xx + y * yx;
		int pixel_y = y0 + x * xy + y * yy;
		if (pixel_x > 640)
			pixel_x = 640;
		if (pixel_x < 0)
			pixel_x = 0;
		if (pixel_y > 480)
			pixel_y = 480;
		if (pixel_y < 0)
			pixel_y = 0;

		vga_mode12h_pixel(color, pixel_x, pixel_y);

		if (D >= 0)
		{
			y += 1;
			D -= 2 * dx;
		}
		D += 2 * dy;
	}
}

void gr_draw_rect(int x0, int y0, int w, int h, char color)
{
	gr_draw_line(x0, y0, x0 + w, y0, color);
	gr_draw_line(x0, y0 + h, x0 + w, y0 + h, color);
	gr_draw_line(x0 + w, y0, x0 + w, y0 + h, color);
	gr_draw_line(x0, y0, x0, y0 + h, color);
}

void gr_print(char character)
{
	if ((windowctx->cursor_y + 8) > GR_HEIGHT - 8)
	{
		gr_window_scroll();
	}
	gr_print_character(windowctx->cursor_x, windowctx->cursor_y, character);
	if ((windowctx->cursor_x + 8) > GR_WIDTH - 8)
	{
		windowctx->cursor_x = 0;
		windowctx->cursor_y += 8;
	}
	else
	{
		windowctx->cursor_x += 8;
	}

	gr_print_character(windowctx->cursor_x, windowctx->cursor_y, '_');
}

void gr_window_print(const char *string)
{
	for (int i = 0; i < strlen((char *)string); i++)
	{
		gr_print(string[i]);
	}
}

void gr_window_scroll(void)
{
	/*
	/ read 1nd character line in first[GR_WIDTH / 8]
	/ read 2nd character line in second[GR_WIDTH / 8]
	/ skip if both first and second are empty
	/ read each character from first and second array
	/ check if pixel needs to be set or cleared
	/ remap the window character buffer
	*/

	if (windowctx->cursor_y < 8)
	{
		return;
	}

	char *tmp = (char *)malloc((GR_HEIGHT / 8) * (GR_WIDTH / 8));
	memcpy(&windowctx->charbuf[GR_WIDTH / 8], tmp, ((GR_HEIGHT / 8) * (GR_WIDTH / 8)) - (GR_WIDTH / 8));

	for (int row = 0; row < (GR_HEIGHT / 8) - 1; row++)
	{
		char *first = (char *)malloc(GR_WIDTH / 8);
		char *second = (char *)malloc(GR_WIDTH / 8);

		memcpy(&windowctx->charbuf[row * GR_WIDTH / 8], first, GR_WIDTH / 8);
		memcpy(&windowctx->charbuf[(row + 1) * GR_WIDTH / 8], second, GR_WIDTH / 8);

		for (int col = 0; col < GR_WIDTH / 8; col++)
		{
			char a = first[col];
			char b = second[col];
			if (a == 0 && b == 0)
			{
				continue;
			}

			char *bitmapa = font8x8_basic[(int)a];
			char *bitmapb = font8x8_basic[(int)b];

			int cx, cy;
			int x = gr_window_get_x(col + (row * (GR_WIDTH) / 8));
			int y = gr_window_get_y(col + (row * (GR_WIDTH) / 8));
			for (cx = 0; cx < 8; cx++)
			{
				for (cy = 0; cy < 8; cy++)
				{
					int seta, setb;
					seta = bitmapa[cx] & 1 << cy;
					setb = bitmapb[cx] & 1 << cy;
					if (seta != setb)
					{
						vga_mode12h_pixel((setb && !seta) ? WHITE : BLACK, (u16)x + cy, (u16)y + cx);
					}
				}
			}
		}
	}

	windowctx->charbuf = tmp;
	windowctx->cursor_y -= 8;
}

int gr_window_get_buffer_idx(int x, int y)
{
	return (x / 8) + ((y / 8) * (GR_WIDTH / 8));
}

// x = index % width;
// y = index / width;
int gr_window_get_x(int idx)
{
	return (idx % (GR_WIDTH / 8)) * 8;
}

int gr_window_get_y(int idx)
{
	return (idx / (GR_WIDTH / 8)) * 8;
}