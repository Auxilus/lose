#include "timer.h"
#include "../drivers/serial.h"
#include "isr.h"
#include "../utils/console.h"

volatile u32 tick = 0;

static void timer_callback(registers_t *regs) {
	tick++;
	return;
}

void init_timer(u32 freq) {
	console_pre_print("TIMER: registering IRQ0/timer_callback\n");
	register_interrupt_handler(IRQ0, timer_callback);

	// hardware clock at 1193180 Hz
	u32 divisor = 1193180 / freq;
	u8 low  = (u8)(divisor & 0xFF);
	// u8 high = (u8)( (divisor >> 8) & 0xFF);
	u8 high = (u8)((divisor & 0xFF00) >>8);
	
	// send PIT command 0x36
	//     00           11                 011                   0
	// channel 0 - lobyte/hibyte - square wave generator - 16-bit binary
	console_pre_print("TIMER: PIT set channel 0 [square wave generator mode]\n");
	port_byte_out(PIT_COMMAND, 0x36);
	port_byte_out(PIT_CHANNEL0_DATA, low);
	port_byte_out(PIT_CHANNEL0_DATA, high);

	// PIT read back
	// 11000010
	console_pre_print("TIMER: PIT read-back\n");
	port_byte_out(PIT_COMMAND, 0xc2);
	u8 pit_channel_status = port_byte_in(PIT_CHANNEL0_DATA);

	char str[64];
	sprintf(str, "TIMER: timer enabled [%d Hz] read-back 0x%x (should be 0xb6)\n", freq, pit_channel_status);
	
	console_pre_print(str);
}

void timer_sleep(int seconds)
{
	int total = tick + (seconds * FREQ);
	// FIXME: figure out why we need enable interrupts again to get the ticks working
	asm volatile("sti");
	while (tick < total);
	return;
}
