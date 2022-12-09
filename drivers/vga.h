#ifndef _DRIVER_VGA
#define _DRIVER_VGA

#include "../cpu/types.h"

#define bit_get(p,m) ((p) & (m))
#define bit_set(p,m) ((p) |= (m))
#define bit_clear(p,m) ((p) &= ~(m))
#define bit_flip(p,m) ((p) ^= (m))
#define bit_write(p,m,v) ((v) ? bit_set(p,m) : bit_clear(p,m))

// VGA graphics controller ports
#define VGA_GC_IDX	0x3ce
#define VGA_GC_DATA 0x3cf

// VGA sequencer ports
#define VGA_SC_IDX	0x3c4
#define VGA_SC_DATA 0x3c5

void vga_set_pixel(int pos_x, int pos_y, u8 VGA_COLOR);
int vga_mode12h_pixel(u8 color, u16 x, u16 y);
void vga_set_plane(u8 p);

#endif
