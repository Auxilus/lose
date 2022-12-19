#ifndef _DRIVER_SERIAL
#define _DRIVER_SERIAL

#define SERIAL_COM1 0x3f8

#include "ports.h"

int		serial_init(void);
int		serial_received(void);
char	serial_read(void);
int		serial_is_transmit_empty(void);
void	serial_write(char ch);
void	serial_print(char *message);
void	serial_color_off(void);
void	serial_color_on(void);

#endif
