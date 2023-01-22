#include "console.h"
#include "../kernel/graphics.h"
#include "../drivers/serial.h"

static int enable_gr_print = 0;

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

void console_set_enable_gr_print(int val)
{
  enable_gr_print = val;
}

int console_get_enable_gr_print(void)
{
  return enable_gr_print;
}