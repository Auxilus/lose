#include "ports.h"

u8 port_byte_in (u16 port) {
	u8 result;
	/* Inline assembler syntax
	 * !! Notice how the source and destination registers are switched from NASM !!
	 *
	 * '"=a" (result)'; set '=' the C variable '(result)' to the value of register e'a'x
	 * '"d" (port)': map the C variable '(port)' into e'd'x register
	 *
	 * Inputs and outputs are separated by colons
	 */
	__asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
	return result;
}

void port_byte_out (u16 port, u8 data) {
	/* Notice how here both registers are mapped to C variables and
	 * nothing is returned, thus, no equals '=' in the asm syntax 
	 * However we see a comma since there are two variables in the input area
	 * and none in the 'return' area
	 */
	__asm__("out %%al, %%dx" : : "a" (data), "d" (port));
}

u16 port_word_in (u16 port) {
	u16 result;
	__asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
	return result;
}

void port_word_out (u16 port, u16 data) {
	__asm__("out %%ax, %%dx" : : "a" (data), "d" (port));
}

void port_long_out(u16 portid, u32 value)
{
	asm volatile("outl %%eax, %%dx": :"d" (portid), "a" (value));
}

u32 port_long_in(u16 portid)
{
	u32 ret;
	asm volatile("inl %%dx, %%eax":"=a"(ret):"d"(portid));
	return ret;
}
