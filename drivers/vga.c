#include "vga.h"
#include "ports.h"

void vga_set_pixel(int pos_x, int pos_y, u8 VGA_COLOR)
{
	u8* location = (u8*)0xA0000 + 320 * pos_y + pos_x;
	*location = VGA_COLOR;
}

int vga_mode12h_pixel(u8 color, u16 x, u16 y)
{
	if((x>=640) || (y >= 480))
		return -1;
	unsigned char *fb = (unsigned char *) 0xA0000;
	unsigned int offset = (y * 640 + x)/8;
	unsigned bit_no = x % 8;
	for(u8 p = 3; p < 4; p--)
	{
		set_plane(p);
		bit_write(fb[offset], 1<<(7-bit_no), (bit_get(color, 1 << p)));
	}
	return 0;
}

void set_plane(u8 p)
{
	unsigned char pmask;

	p &= 3;
	pmask = 1 << p;

	/* set read plane */
	port_byte_out(VGA_GC_IDX, 4);
	port_byte_out(VGA_GC_DATA, p);

	/* set write plane */
	port_byte_out(VGA_SC_IDX, 2);
	port_byte_out(VGA_SC_DATA, pmask);
}
