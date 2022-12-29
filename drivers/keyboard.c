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
    0x08, // backspace
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
		'`',
    ' ', // 42 lshift
    '\\', // backslash
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

char shifted_key_map[0x100] = {
    ' ', //Invalid,
    ' ', //Escape,
    '!', //ExclamationPoint,
    '@', //AtSign,
    '#', //Hashtag,
    '$', //Dollar,
    '%', //Percent,
    '^', //Circumflex,
    '&', //Ampersand,
    '*', //Asterisk,
    '(', //LeftParen,
    ')', //RightParen,
    '_', //Underscore,
    '+', //Plus,
    0x08, //Backspace,
    '\t', //Tab,
    'Q',
    'W',
    'E',
    'R',
    'T',
    'Y',
    'U',
    'I',
    'O',
    'P',
    '{', //LeftBrace,
    '}', //RightBrace,
    '\n', //Return,
    ' ', //Control,
    'A',
    'S',
    'D',
    'F',
    'G',
    'H',
    'J',
    'K',
    'L',
    ':', //Colon,
    '\"', //DoubleQuote,
    '~', //Tilde,
    ' ', // 42
    '|', //Pipe,
    'Z',
    'X',
    'C',
    'V',
    'B',
    'N',
    'M',
    '<', //LessThan,
    '>', //GreaterThan,
    '?', //QuestionMark,
    ' ', //RightShift, // 54
    '*', //Asterisk,
    ' ', //Alt,
    ' ', //Space,    // 57
    ' ', //CapsLock, // 58
		' ', //F
		' ', //F
		' ', //F
		' ', //F
		' ', //F
		' ', //F
		' ', //F
		' ', //F
		' ', //F
		' ', //F
    ' ', //NumLock,
    ' ', //Invalid, // 70
    ' ', //Home,
    ' ', //Up,
    ' ', //PageUp,
    ' ', //Minus,
    ' ', //Left,
    ' ', //Invalid,
    ' ', //Right, // 77
    ' ', //Plus,
    ' ', //End,
    ' ', //Down, // 80
    ' ', //PageDown,
    ' ', //Insert,
    ' ', //Delete, // 83
    ' ', //Invalid,
    ' ', //Invalid,
    ' ', //Pipe,
    ' ', //F11,
    ' ', //F12,
    ' ', //Invalid,
    ' ', //Invalid,
    ' ', //Super,
    ' ', //Invalid,
    ' ', //Menu,
};

static int is_shift = 0;
static int is_ctrl = 0;

u8 scancode2char(int sc)
{
	// TODO: handle these conditions better
	if(sc & (1 << 7)) { return 0; }
	sc &= 0x7f;
	if (sc >= 0x50) { return 0; }
	return is_shift ? shifted_key_map[sc] : unshifted_key_map[sc];
}

void keyboard_callback(registers_t *regs)
{
	port_byte_out(0x20, 0x20);
	u8 scancode = port_byte_in(0x60);
	int sc = (int) scancode;

	// TODO: verify release handler
	if ((scancode & 128) == 128) {

		// check for shift release L & R
		// TODO: don't hardcode scancode values
		if (sc == 170 || sc == 182) { is_shift = 0; }
		if (sc == 157 || sc == 224) { is_ctrl = 0; }

		//char message[128];
		//sprintf(message, "KEYBOARD: key released [%d] [%x]\n", sc, scancode);
		//serial_print(message);
	}
	else {
		// keypress handler
		//char message[128];
		//sprintf(message, "KEYBOARD: key pressed [%d] [%x]\n", sc, scancode);
		//serial_print(message);
		
		// check for shift press L & R
		// TODO: don't hardcode scancode values
		if (sc == 42 || sc == 54) { is_shift = 1; return; }
		if (sc == 29) { is_ctrl = 1; return; }

		if (sc < 0x50) {
			key_event ke;
			ke.letter = scancode2char(sc);
			ke.scancode = sc;
			ke.is_shift = is_shift;
			ke.is_ctrl = is_ctrl;
			kernel_handle_key(ke);
		}
	}
}

void init_keyboard()
{
	register_interrupt_handler(IRQ1, keyboard_callback);
}
