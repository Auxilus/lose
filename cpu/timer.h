#ifndef TIMER_H
#define TIMER_H

#include "../utils/string.h"
#include "types.h"

#define FREQ 1000
#define PIT_COMMAND 0x43
#define PIT_CHANNEL0_DATA 0x40

void init_timer(u32 freq);
void timer_sleep(int seconds);

#endif
