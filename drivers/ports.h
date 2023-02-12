#ifndef _DRIVER_PORTS
#define _DRIVER_PORTS

#include "../cpu/types.h"

u8 port_byte_in(u16 port);
u16 port_word_in(u16 port);
u32 inportl(u16 portid);
void outportl(u16 portid, u32 value);
void port_byte_out(u16 port, u8 data);
void port_word_out(u16 port, u16 data);
static inline void outsw(uint16_t port, void *buf, int ns)
{
  uint16_t *b = (uint16_t *)buf;

  while (ns > 0)
  {
    port_word_out(port, *b++);
    ns--;
  }
}

static inline void insw(uint16_t port, void *buf, int ns)
{
  uint16_t *b = (uint16_t *)buf;

  while (ns > 0)
  {
    *b++ = port_word_in(port);
    ns--;
  }
}

#endif
