#include "vga.h"
#include "ports.h"

unsigned char *fb = (unsigned char *)0xA0000;

// void vga_set_pixel(int pos_x, int pos_y, u8 VGA_COLOR)
// {
// 	u8 *location = (u8 *)0xA0000 + 320 * pos_y + pos_x;
// 	*location = VGA_COLOR;
// }

int vga_mode12h_pixel(u8 color, u16 x, u16 y)
{
	if ((x >= 640) || (y >= 480))
		return -1;

	// set bitmask
	uint8_t bitmask = 1 << ((x & 0x07) ^ 0x07);
	port_word_out(VGA_GC_IDX, 0x08);
	port_byte_out(VGA_GC_DATA, bitmask);

	int offset = y * 80 + x / 8;
	// trigger latching
	volatile uint8_t dummy = *(fb + offset);
	*(fb + offset) = color % 16;
	return 0;
}

// void vga_set_plane(u8 p)
// {
// 	unsigned char pmask;

// 	p &= 3;
// 	pmask = 1 << p;

// 	/* set read plane */
// 	port_byte_out(VGA_GC_IDX, 4);
// 	port_byte_out(VGA_GC_DATA, p);

// 	/* set write plane */
// 	port_byte_out(VGA_SC_IDX, 2);
// 	port_byte_out(VGA_SC_DATA, pmask);
// }

void vga_write_registers(void)
{
	// asm volatile("cli");
	uint8_t *registers = g_640x480x16;
	port_byte_out(VGA_MISC_PORT, *(registers));
	registers++;

	for (uint8_t i = 0; i < 5; i++)
	{
		port_byte_out(VGA_SC_IDX, i);
		port_byte_out(VGA_SC_DATA, *(registers));
		registers++;
	}

	port_byte_out(VGA_CRTC_IDX, 0x03);
	port_byte_out(VGA_CRTC_DATA, port_byte_in(VGA_CRTC_DATA) | 0x80);
	port_byte_out(VGA_CRTC_IDX, 0x11);
	port_byte_out(VGA_CRTC_DATA, port_byte_in(VGA_CRTC_DATA) & ~0x80);

	registers[0x03] = registers[0x03] | 0x80;
	// registers[0x11] = registers[0x11] & ~0x80;

	for (uint8_t i = 0; i < 25; i++)
	{
		port_byte_out(VGA_CRTC_IDX, i);
		port_byte_out(VGA_CRTC_DATA, *(registers));
		registers++;
	}

	for (uint8_t i = 0; i < 9; i++)
	{
		port_byte_out(VGA_GC_IDX, i);
		port_byte_out(VGA_GC_DATA, *(registers));
		registers++;
	}

	for (uint8_t i = 0; i < 21; i++)
	{
		(void)port_byte_in(VGA_ATC_RESET);
		port_byte_out(VGA_ATC_IDX, i);
		port_byte_out(VGA_ATC_WRITE, *(registers));
		registers++;
	}

	port_byte_in(VGA_ATC_RESET);
	port_byte_out(VGA_ATC_IDX, 0x20);

	// asm volatile("sti");

	port_word_out(VGA_GC_IDX, 0x05); // R/W mode
	uint8_t mode = port_byte_in(VGA_GC_DATA) & 0xF4;
	mode |= ((0 & 1) << 3) | (2 & 3);
	port_byte_out(VGA_GC_DATA, mode);
}