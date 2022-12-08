#ifndef _DRIVER_PORTS
#define _DRIVER_PORTS

#include "../cpu/types.h"

#define SERIAL_COM1 0x3f8

u8 port_byte_in(u16 port);
u16 port_word_in(u16 port);
void port_byte_out(u16 port, u8 data);
void port_word_out(u16 port, u16 data);
int init_serial(void);
int serial_received(void);
char read_serial(void);
int is_transmit_empty(void);
void write_serial(char ch);
void print_serial(char *message);

#endif
