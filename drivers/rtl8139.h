#ifndef __RTL8139_H_
#define __RTL8139_H_

#include "../cpu/types.h"

// RTL8139 registers
#define RTL8139_MAC0to5 0x00
#define RTL8139_MAR0to7 0x08
#define RTL8139_RBSTART 0x30
#define RTL8139_CMD 0x37
#define RTL8139_IMR 0x3C
#define RTL8139_ISR 0x3E
#define RTL8139_CONFIG0 0x51
#define RTL8139_CONFIG1 0x52

static uint32_t rtl8139_bar_type;
static uint32_t rtl8139_io_base;
static uint32_t rtl8139_mem_base;
static char *rtl8139_rx_buffer;

int rtl8139_init(void);

#endif