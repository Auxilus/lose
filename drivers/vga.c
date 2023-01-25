#include "vga.h"
#include "ports.h"

void vga_set_pixel(int pos_x, int pos_y, u8 VGA_COLOR)
{
	u8 *location = (u8 *)0xA0000 + 320 * pos_y + pos_x;
	*location = VGA_COLOR;
}

int vga_mode12h_pixel(u8 color, u16 x, u16 y)
{
	if ((x >= 640) || (y >= 480))
		return -1;
	port_word_out(sequencerIndexPort, 0x02);
	port_byte_out(sequencerDataPort, 0xff);
	unsigned char *fb = (unsigned char *)0xA0000;
	unsigned int offset = (y * 640 + x) / 8;
	unsigned bit_no = x % 8;
	bit_write(fb[offset], 1 << (7 - bit_no), (bit_get(color, 1 << 0)));
	return 0;
}

void vga_set_plane(u8 p)
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

void vga_write_registers(void)
{
	uint8_t *registers = g_640x480x16;
	port_byte_out(miscPort, *(registers));
	registers++;

	for (uint8_t i = 0; i < 5; i++)
	{
		port_byte_out(sequencerIndexPort, i);
		port_byte_out(sequencerDataPort, *(registers));
		registers++;
	}

	port_byte_out(crtcIndexPort, 0x03);
	port_byte_out(crtcDataPort, port_byte_in(crtcDataPort) | 0x80);
	port_byte_out(crtcIndexPort, 0x11);
	port_byte_out(crtcDataPort, port_byte_in(crtcDataPort) & ~0x80);

	registers[0x03] = registers[0x03] | 0x80;
	// registers[0x11] = registers[0x11] & ~0x80;

	for (uint8_t i = 0; i < 25; i++)
	{
		port_byte_out(crtcIndexPort, i);
		port_byte_out(crtcDataPort, *(registers));
		registers++;
	}

	for (uint8_t i = 0; i < 9; i++)
	{
		port_byte_out(graphicsControllerIndexPort, i);
		port_byte_out(graphicsControllerDataPort, *(registers));
		registers++;
	}

	for (uint8_t i = 0; i < 21; i++)
	{
		(void)port_byte_in(attributeControllerResetPort);
		port_byte_out(attributeControllerIndexPort, i);
		port_byte_out(attributeControllerWritePort, *(registers));
		registers++;
	}

	port_byte_in(attributeControllerResetPort);
	port_byte_out(attributeControllerIndexPort, 0x20);
}