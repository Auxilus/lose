#include "keyboard.h"
#include "../drivers/serial.h"
#include "ports.h"
#include "../utils/string.h"
#include "../kernel/kernel.h"
#include "../cpu/isr.h"

char unshifted_key_map[0x80] = {
    '0',
    ' ', //Key_Escape,
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '0',
    '-',
    '=',
    ' ', // backspace
    '\t', // 15
    'q',
    'w',
    'e',
    'r',
    't',
    'y',
    'u',
    'i',
    'o',
    'p',
    '[',
    ']',
    0x0a,  // 28
    ' ', // 29 ctrl
    'a',
    's',
    'd',
    'f',
    'g',
    'h',
    'j',
    'k',
    'l',
    ';',
    '\'',
		'\`',
    ' ', // 42 lshift
    ' ', // backslash
    'z',
    'x',
    'c',
    'v',
    'b',
    'n',
    'm',
    ',',
    '.',
    '/',
    ' ', // 54 rshift
    '*',
    ' ',      // 56 alt
    ' ',    // 57
    ' ', // 58 caps lock
		' ', // Fkeys
		' ', // Fkeys
		' ', // Fkeys
		' ', // Fkeys
		' ', // Fkeys
		' ', // Fkeys
		' ', // Fkeys
		' ', // Fkeys
		' ', // Fkeys
		' ', // Fkeys
    ' ', //NumLock,
    '0', // 70
    ' ',//Key_Home,
    ' ',//Key_Up,
    ' ',//Key_PageUp,
    ' ',//Key_Minus,
    ' ',//Key_Left,
    '0',
    ' ', //Key_Right, // 77
    ' ', //Key_Plus,
    ' ', //Key_End,
    ' ', //Key_Down, // 80
    ' ', //Key_PageDown,
    ' ', //Key_Insert,
    ' ', //Key_Delete, // 83
    '0',
    '0',
    ' ', //Key_Backslash,
    ' ', //Key_F11,
    ' ', //Key_F12,
    '0',
    '0',
    ' ', //Key_Super,
    '0',
    ' ', //Key_Menu,
};

u8 scancode2char(int sc)
{
	// TODO: handle these conditions better
	if(sc & (1 << 7)) { return 0; }
	sc &= 0x7f;
	if (sc >= 0x50) { return 0; }
	return unshifted_key_map[sc];
}

void keyboard_callback(registers_t *regs)
{
	u8 scancode = port_byte_in(0x60);
	int sc = (int) scancode;

	// TODO: handle keyrelease
	if (sc < 0x50) {
		kernel_handle_key(scancode2char(sc));
	}
}

void init_keyboard()
{
	register_interrupt_handler(IRQ1, keyboard_callback);
}
