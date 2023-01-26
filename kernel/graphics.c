#include "graphics.h"
#include "font8x8_basic.h"
#include "../drivers/vga.h"
#include "../drivers/ports.h"
#include "../drivers/serial.h"
#include "../utils/console.h"
#include "../utils/string.h"
#include "../utils/console.h"

int graphics_abs(int value) { return value < 0 ? 0 - value : value; }

void gr_set_color_fg(u8 color)
{
	gr_color_fg = color;
}

void gr_init_graphics(void)
{
	gr_color_fg = WHITE;
	serial_print("GRAPHICS: init graphics\n");
	gr_clear_screen();
	serial_print("GRAPHICS: init window context\n");
	windowctx->cursor_x = 0;
	windowctx->cursor_y = 0;
	windowctx->charbuf = (char *)malloc((GR_HEIGHT / 8) * (GR_WIDTH / 8));
	memset(windowctx->charbuf, 0, (GR_HEIGHT / 8) * (GR_WIDTH / 8));
	gr_print_character(0, 0, '_', 1);
	serial_print("GRAPHICSL: init done\n");
}

void gr_clear_screen(void)
{
	serial_print("GRAPHICS: clearing plane framebuffer\n");

	char s[1];
	port_word_out(VGA_SEQ_IDX, 0x02);
	port_byte_out(VGA_SEQ_DATA, 0xff);
	memset((u8 *)GR_START, BLACK, 64 * 1024);
	memset(windowctx->charbuf, 0, (GR_HEIGHT / 8) * (GR_WIDTH / 8));
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
			gr_print_character(cx, y, character, 0);
			cx = cx + 8;
		}
	}
}

void gr_print_character(int x, int y, int character, int skipAdvance)
{
	int cx, cy;
	int set;
	char *bitmap = font8x8_basic[character];

	if ((y + 8) > GR_HEIGHT)
	{
		gr_window_scroll();
		x = windowctx->cursor_x;
		y = windowctx->cursor_y;
	}

	if ((x + 8) > GR_WIDTH)
	{
		x = 0;
		y += 8;
		windowctx->cursor_x = x;
		windowctx->cursor_y = y;
	}

	int winbuf_idx = gr_window_get_buffer_idx(x, y);
	windowctx->charbuf[winbuf_idx] = (char)character;

	if (!skipAdvance)
	{
		windowctx->cursor_x += 8;
	}
	for (cx = 0; cx < 8; cx++)
	{
		for (cy = 0; cy < 8; cy++)
		{
			set = bitmap[cx] & 1 << cy;
			vga_mode12h_pixel(set ? gr_color_fg : BLACK, (u16)x + cy, (u16)y + cx);
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

	if (character == '\n')
	{

		windowctx->cursor_x = 0;
		windowctx->cursor_y += 8;
	}
	else if (character == 0x08)
	{
		if (windowctx->cursor_x > 8)
		{
			windowctx->cursor_x -= 8;
			gr_print_character(windowctx->cursor_x, windowctx->cursor_y, ' ', 1);
		}
	}

	else
	{
		gr_print_character(windowctx->cursor_x, windowctx->cursor_y, character, 0);
	}
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

	int nW = (GR_WIDTH / 8);
	int nH = (GR_HEIGHT / 8);

	if (windowctx->cursor_y < 8)
	{
		return;
	}

	char *tmp = (char *)malloc((nH) * (nW));
	memcpy(&windowctx->charbuf[nW], tmp, ((nH) * (nW)) - (nW));

	for (int row = 0; row < (nH)-1; row++)
	{
		char *first = (char *)malloc(nW);
		char *second = (char *)malloc(nW);

		memcpy(&windowctx->charbuf[row * nW], first, nW);
		memcpy(&windowctx->charbuf[(row + 1) * nW], second, nW);

		for (int col = 0; col < nW; col++)
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
						vga_mode12h_pixel((setb && !seta) ? gr_color_fg : BLACK, (u16)x + cy, (u16)y + cx);
					}
				}
			}
		}
	}

	char *last = (char *)malloc(nW);
	memcpy(&windowctx->charbuf[((nH)-1) * nW], last, nW);

	if (last != "")
	{
		for (int last_col = 0; last_col < nW; last_col++)
		{
			char a = last[last_col];
			if (last_col == '\0')
			{
				continue;
			}

			int cx, cy;
			char *bitmapa = font8x8_basic[(int)a];
			int x = gr_window_get_x(last_col + (((nH)-1) * (GR_WIDTH) / 8));
			int y = gr_window_get_y(last_col + (((nH)-1) * (GR_WIDTH) / 8));
			for (cx = 0; cx < 8; cx++)
			{
				for (cy = 0; cy < 8; cy++)
				{
					int seta = bitmapa[cx] & 1 << cy;
					if (seta)
					{
						vga_mode12h_pixel(BLACK, (u16)x + cy, (u16)y + cx);
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