#include "console.h"
#include "../kernel/graphics.h"
#include "../drivers/serial.h"

void console_pre_init(void)
{
  enable_gr_print = 1;
}

void console_pre_print(char *str)
{
  if (enable_gr_print)
  {
    gr_window_print(str);
  }
  serial_print(str);
}