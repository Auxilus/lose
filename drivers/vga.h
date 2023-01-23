#ifndef _DRIVER_VGA
#define _DRIVER_VGA

#include "../cpu/types.h"

//REGISTER VALUES
unsigned static const char g_640x480x16[] = {
/* MISC */
  0xE3, 
/* SEQ */
  0x03, 0x01, 0x0F, 0x00, 0x06, 
/* CRTC */
  0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0x0B, 0x3E,
  0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xEA, 0x8C, 0xDF, 0x28, 0x00, 0xE7, 0x04, 0xE3,
  0xFF, 
/* GC */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0F,
  0xFF, 
/* AC */
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
  0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
  0x01, 0x00, 0x0F, 0x00, 0x00, 
};

#define miscPort 0x3c2
#define crtcIndexPort 0x3d4
#define crtcDataPort 0x3d5
#define crtcIndexPortText 0x3b4
#define crtcDataPortText 0x3b5
#define sequencerIndexPort 0x3c4
#define sequencerDataPort 0x3c5
#define graphicsControllerIndexPort 0x3ce
#define graphicsControllerDataPort 0x3cf
#define attributeControllerIndexPort 0x3c0
#define attributeControllerReadPort 0x3c1
#define attributeControllerWritePort 0x3c0
#define attributeControllerResetPort 0x3da

#define TRANSPARENT		0xFF
#define BLACK					0
#define BLUE					1
#define GREEN					2
#define CYAN					3
#define RED						4
#define PURPLE				5
#define BROWN					6
#define LTGRAY				7
#define DKGRAY				8
#define LTBLUE				9
#define LTGREEN				10
#define LTCYAN				11
#define LTRED					12
#define LTPURPLE			13
#define YELLOW				14
#define WHITE					15

#define bit_get(p, m) ((p) & (m))
#define bit_set(p, m) ((p) |= (m))
#define bit_clear(p, m) ((p) &= ~(m))
#define bit_flip(p, m) ((p) ^= (m))
#define bit_write(p, m, v) ((v) ? bit_set(p, m) : bit_clear(p, m))

// VGA graphics controller ports
#define VGA_GC_IDX 0x3ce
#define VGA_GC_DATA 0x3cf

// VGA sequencer ports
#define VGA_SC_IDX 0x3c4
#define VGA_SC_DATA 0x3c5
#define NULL '\0'

static char *vga_back_buffer[] = {NULL, NULL, NULL, NULL};
static uint8_t m_cval[256];
static uint8_t m_cmap[256];

void vga_init(void);
void vga_write_registers(void);
void vga_set_pixel(int pos_x, int pos_y, u8 VGA_COLOR);
int vga_mode12h_pixel(u8 color, u16 x, u16 y);
void vga_swap_buffers(void);
int vga_buffered_mode12h_pixel(u8 color, u16 x, u16 y);
void vga_set_plane(u8 p);
uint8_t vga_get_color(uint8_t cname);

#endif
