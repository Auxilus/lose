#include "vga.h"
#include "ports.h"
#include "../utils/mem.h"

void vga_set_pixel(int pos_x, int pos_y, u8 VGA_COLOR)
{
	u8 *location = (u8 *)0xA0000 + 320 * pos_y + pos_x;
	*location = VGA_COLOR;
}

int vga_mode12h_pixel(u8 color, u16 x, u16 y)
{
	if (x < 0 || x > 640 || y < 0 || y > 480)
	{
		return;
	}

	uint8_t bitmask = 1 << ((x & 0x07) ^ 0x07);
	port_byte_out(VGA_GC_IDX, 0x08);
	port_byte_out(VGA_GC_DATA, bitmask);

	int offset = y * 80 + x / 8;
	volatile uint8_t dummy = *(((uint8_t *)0xa0000) + offset); // load latches
	*(((uint8_t *)0xa0000) + offset) = 17;//vga_get_color((uint8_t)color) % 16;
}

uint8_t vga_get_color(uint8_t cname)
{
	return m_cval[m_cmap[cname]];
}

int lol_failed_attempt_at_vga_mode12_laggy_as_last_breath(u8 color, u16 x, u16 y)
{
	if ((x >= 640) || (y >= 480))
		return -1;
	unsigned char *fb = (unsigned char *)0xA0000;
	unsigned int offset = (y * 640 + x) / 8;
	unsigned bit_no = x % 8;
	for (u8 p = 3; p < 4; p--)
	{
		vga_set_plane(p);
		bit_write(fb[offset], 1 << (7 - bit_no), (bit_get(color, 1 << p)));
	}
	return 0;
}

int vga_buffered_mode12h_pixel(u8 color, u16 x, u16 y)
{
	if ((x >= 640) || (y >= 480))
		return -1;
	unsigned int offset = (y * 640 + x) / 8;
	unsigned bit_no = x % 8;
	for (uint8_t p = 3; p < 4; p--)
	{
		bit_write(vga_back_buffer[p][offset], 1 << (7 - bit_no), (bit_get(color, 1 << p)));
	}
	return 0;
}

void vga_swap_buffers()
{
	unsigned char *fb = (unsigned char *)0xA0000;
	for (uint8_t p = 3; p < 4; p--)
	{
		vga_set_plane(p);
		memcpy(vga_back_buffer[p], fb, 640 * 480 / 8);
	}
	// memcpy(VGA_VRAM, mode13h_buffer, MODE13H_HEIGHT * MODE13H_WIDTH);
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

void vga_init()
{
	// for (int i = 3; i >= 0; i--)
	// {
	// 	vga_back_buffer[i] = (char *)malloc(480 * 640 / 8);
	// 	if (vga_back_buffer[i] == NULL)
	// 	{
	// 		for (int j = 3; j >= i; j--)
	// 		{
	// 			free(sizeof(vga_back_buffer[j]));
	// 			vga_back_buffer[j] = NULL;
	// 		}
	// 		return;
	// 	}
	// }
	// vga_write_registers();
	port_byte_out(VGA_GC_IDX, 0x05); // R/W mode
	uint8_t mode = port_byte_in(VGA_GC_DATA) & 0xF4;
	mode |= ((0 & 1) << 3) | (2 & 3);
	port_byte_out(VGA_GC_DATA, mode);
	for (int i = 0; i < 256; i++)
		m_cval[i] = i;
	for (int i = 0; i < 256; i++)
		m_cmap[i] = i;
}

void vga_write_registers(void)
{
	uint8_t *registers = (uint8_t *)g_640x480x16;
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
		port_byte_out(VGA_GC_IDX, i);
		port_byte_out(VGA_GC_DATA, *(registers));
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