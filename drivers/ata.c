#include "ata.h"
#include "ports.h"

void read_sector(int addr, int count, char *buffer)
{
  // FIXME: assert count <= 255

  // TODO: wait for ready

  char data;
  // how many sector
  data = count;
  port_byte_out(ATA_BASE + 2, data);

  // set addr
  data = addr & 0xff;
  port_byte_out(ATA_BASE + 3, data);
  data = addr >> 8 & 0xff;
  port_byte_out(ATA_BASE + 4, data);
  data = addr >> 16 & 0xff;
  port_byte_out(ATA_BASE + 5, data);
  data = (addr >> 24 & 0x0f) | 0xe0;
  port_byte_out(ATA_BASE + 6, data);

  // read command
  data = 0x20;
  port_byte_out(ATA_BASE + 7, data);

  // pool
  data = port_byte_in(ATA_BASE + 7);
  while ((data & 0x88) != 0x08)
  {
    data = port_byte_in(ATA_BASE + 7);
  }

  // read data
  int loop = count * 512 / 2;
  uint16_t d;
  int p = 0;
  while (loop--)
  {
    d = port_word_in(ATA_BASE);
    buffer[p++] = (char)d & 0x00ff;
    buffer[p++] = (char)(d >> 8);
  }
}