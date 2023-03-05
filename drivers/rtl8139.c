#include "rtl8139.h"
#include "ports.h"
#include "../utils/console.h"
#include "pci.h"
#include "../cpu/isr.h"
#include "../utils/string.h"
#include "../utils/mem.h"

static void rtl8139_callback(registers_t *regs)
{
  console_pre_print("NET (RTL8139): activity\n");
}

int rtl8139_init(void)
{
  console_pre_print("NET (RTL8139): init\n");

  console_pre_print("NET (RTL8139): searching for device\n");
  pci_device_search *devices = pci_find_by_type(0x0002, 0x0000, 0x00);
  if (devices->count > 0)
  {
    console_pre_print("NET (RTL8139): device found\n");

    rtl8139_bar_type = devices->devices[0]->BAR0 & 0x1;
    rtl8139_io_base = (rtl8139_bar_type != 0) ? devices->devices[0]->BAR0 & (~0x3) : devices->devices[0]->BAR0 & (~0xf);

    char bar0[80];
    sprintf(bar0, "NET (RTL8139): will use %s access with base 0x%04x (int %u)\n",
            (rtl8139_bar_type == 0) ? "memory" : "port",
            rtl8139_io_base,
            devices->devices[0]->interrupt_line);
    console_pre_print(bar0);

    console_pre_print("NET (RTL8139): powering on, resetting RX, TX buffers\n");
    // power on
    port_byte_out(rtl8139_io_base + RTL8139_CONFIG1, 0x0);
    // reset
    port_byte_out(rtl8139_io_base + RTL8139_CMD, 0x10);
    while ((port_byte_in(rtl8139_io_base + RTL8139_CMD) & 0x10) != 0)
    {
    }

    // init receive buffer
    rtl8139_rx_buffer = (char *)malloc(8192 + 16);
    port_byte_out(rtl8139_io_base + RTL8139_RBSTART, (uint32_t)rtl8139_rx_buffer);
    port_word_out(rtl8139_io_base + RTL8139_IMR, 0x0005);
    port_long_out(rtl8139_io_base + 0x44, 0xf | (1 << 7));
    port_byte_out(rtl8139_io_base + 0x37, 0x0C);
    register_interrupt_handler(IRQ0 + devices->devices[0]->interrupt_line, rtl8139_callback);
  }
}
