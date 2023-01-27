#ifndef TIMER_H
#define TIMER_H

#include "../utils/string.h"
#include "types.h"

#define FREQ 1000

void init_timer(u32 freq);
void timer_sleep(int seconds);

#endif
