#ifndef __R3N_INPUTS_H__
#define __R3N_INPUTS_H__

#define _R3KIT_INCLUDE_
#include <r3ngin/include/r3n.core/defines.h>

#define R3N_DEFINE_KEY(name, code) R3N_KEY_##name = code

typedef enum R3N_Mouse_Button {
    R3N_MBUTTON_LEFT,
    R3N_MBUTTON_RIGHT,
    R3N_MBUTTON_MIDDLE,
    R3N_MBUTTON_SIDE1,
    R3N_MBUTTON_SIDE2,
    R3N_MOUSE_MAX_BUTTONS
} R3N_Mouse_Button;

typedef enum R3N_Keyboard_Key {
    R3N_DEFINE_KEY(BACKSPACE, 0x08),
    R3N_DEFINE_KEY(ENTER, 0x0D),
    R3N_DEFINE_KEY(TAB, 0x09),
    R3N_DEFINE_KEY(SHIFT, 0x10),
    R3N_DEFINE_KEY(CONTROL, 0x11),
    R3N_DEFINE_KEY(PAUSE, 0x13),
    R3N_DEFINE_KEY(CAPITAL, 0x14),
    R3N_DEFINE_KEY(ESCAPE, 0x1B),
    R3N_DEFINE_KEY(CONVERT, 0x1C),
    R3N_DEFINE_KEY(NONCONVERT, 0x1D),
    R3N_DEFINE_KEY(ACCEPT, 0x1E),
    R3N_DEFINE_KEY(MODECHANGE, 0x1F),
    R3N_DEFINE_KEY(SPACE, 0x20),
    R3N_DEFINE_KEY(PRIOR, 0x21),
    R3N_DEFINE_KEY(NEXT, 0x22),
    R3N_DEFINE_KEY(END, 0x23),
    R3N_DEFINE_KEY(HOME, 0x24),
    R3N_DEFINE_KEY(LEFT, 0x25),
    R3N_DEFINE_KEY(UP, 0x26),
    R3N_DEFINE_KEY(RIGHT, 0x27),
    R3N_DEFINE_KEY(DOWN, 0x28),
    R3N_DEFINE_KEY(SELECT, 0x29),
    R3N_DEFINE_KEY(PRINT, 0x2A),
    R3N_DEFINE_KEY(EXECUTE, 0x2B),
    R3N_DEFINE_KEY(SNAPSHOT, 0x2C),
    R3N_DEFINE_KEY(INSERT, 0x2D),
    R3N_DEFINE_KEY(DELETE, 0x2E),
    R3N_DEFINE_KEY(HELP, 0x2F),
    R3N_DEFINE_KEY(A, 0x41),
    R3N_DEFINE_KEY(B, 0x42),
    R3N_DEFINE_KEY(C, 0x43),
    R3N_DEFINE_KEY(D, 0x44),
    R3N_DEFINE_KEY(E, 0x45),
    R3N_DEFINE_KEY(F, 0x46),
    R3N_DEFINE_KEY(G, 0x47),
    R3N_DEFINE_KEY(H, 0x48),
    R3N_DEFINE_KEY(I, 0x49),
    R3N_DEFINE_KEY(J, 0x4A),
    R3N_DEFINE_KEY(K, 0x4B),
    R3N_DEFINE_KEY(L, 0x4C),
    R3N_DEFINE_KEY(M, 0x4D),
    R3N_DEFINE_KEY(N, 0x4E),
    R3N_DEFINE_KEY(O, 0x4F),
    R3N_DEFINE_KEY(P, 0x50),
    R3N_DEFINE_KEY(Q, 0x51),
    R3N_DEFINE_KEY(R, 0x52),
    R3N_DEFINE_KEY(S, 0x53),
    R3N_DEFINE_KEY(T, 0x54),
    R3N_DEFINE_KEY(U, 0x55),
    R3N_DEFINE_KEY(V, 0x56),
    R3N_DEFINE_KEY(W, 0x57),
    R3N_DEFINE_KEY(X, 0x58),
    R3N_DEFINE_KEY(Y, 0x59),
    R3N_DEFINE_KEY(Z, 0x5A),
    R3N_DEFINE_KEY(LWIN, 0x5B),
    R3N_DEFINE_KEY(RWIN, 0x5C),
    R3N_DEFINE_KEY(APPS, 0x5D),
    R3N_DEFINE_KEY(SLEEP, 0x5F),
    R3N_DEFINE_KEY(NUMPAD0, 0x60),    // wrong on win32????
    R3N_DEFINE_KEY(NUMPAD1, 0x61),    // wrong on win32????
    R3N_DEFINE_KEY(NUMPAD2, 0x62),    // wrong on win32????
    R3N_DEFINE_KEY(NUMPAD3, 0x63),    // wrong on win32????
    R3N_DEFINE_KEY(NUMPAD4, 0x64),    // wrong on win32????
    R3N_DEFINE_KEY(NUMPAD5, 0x65),    // wrong on win32????
    R3N_DEFINE_KEY(NUMPAD6, 0x66),    // wrong on win32????
    R3N_DEFINE_KEY(NUMPAD7, 0x67),    // wrong on win32????
    R3N_DEFINE_KEY(NUMPAD8, 0x68),    // wrong on win32????
    R3N_DEFINE_KEY(NUMPAD9, 0x69),    // wrong on win32????
    R3N_DEFINE_KEY(MULTIPLY, 0x6A),
    R3N_DEFINE_KEY(ADD, 0x6B),
    R3N_DEFINE_KEY(SEPARATOR, 0x6C),
    R3N_DEFINE_KEY(SUBTRACT, 0x6D),
    R3N_DEFINE_KEY(DECIMAL, 0x6E),
    R3N_DEFINE_KEY(DIVIDE, 0x6F),
    R3N_DEFINE_KEY(F1, 0x70),
    R3N_DEFINE_KEY(F2, 0x71),
    R3N_DEFINE_KEY(F3, 0x72),
    R3N_DEFINE_KEY(F4, 0x73),
    R3N_DEFINE_KEY(F5, 0x74),
    R3N_DEFINE_KEY(F6, 0x75),
    R3N_DEFINE_KEY(F7, 0x76),
    R3N_DEFINE_KEY(F8, 0x77),
    R3N_DEFINE_KEY(F9, 0x78),
    R3N_DEFINE_KEY(F10, 0x79),
    R3N_DEFINE_KEY(F11, 0x7A),
    R3N_DEFINE_KEY(F12, 0x7B),
    R3N_DEFINE_KEY(F13, 0x7C),
    R3N_DEFINE_KEY(F14, 0x7D),
    R3N_DEFINE_KEY(F15, 0x7E),
    R3N_DEFINE_KEY(F16, 0x7F),
    R3N_DEFINE_KEY(F17, 0x80),
    R3N_DEFINE_KEY(F18, 0x81),
    R3N_DEFINE_KEY(F19, 0x82),
    R3N_DEFINE_KEY(F20, 0x83),
    R3N_DEFINE_KEY(F21, 0x84),
    R3N_DEFINE_KEY(F22, 0x85),
    R3N_DEFINE_KEY(F23, 0x86),
    R3N_DEFINE_KEY(F24, 0x87),
    R3N_DEFINE_KEY(NUMLOCK, 0x90),
    R3N_DEFINE_KEY(SCROLL, 0x91),
    R3N_DEFINE_KEY(NUMPAD_EQUAL, 0x92),
    // R3N_DEFINE_KEY(LSHIFT, 0xA0),
    // R3N_DEFINE_KEY(RSHIFT, 0xA1),
    R3N_DEFINE_KEY(LCONTROL, 0xA2),
    R3N_DEFINE_KEY(RCONTROL, 0xA3),
    R3N_DEFINE_KEY(LMENU, 0xA4),
    R3N_DEFINE_KEY(RMENU, 0xA5),
    R3N_DEFINE_KEY(SEMICOLON, 0xBA),
    R3N_DEFINE_KEY(PLUS, 0xBB),
    R3N_DEFINE_KEY(COMMA, 0xBC),
    R3N_DEFINE_KEY(MINUS, 0xBD),
    R3N_DEFINE_KEY(PERIOD, 0xBE),
    R3N_DEFINE_KEY(SLASH, 0xBF),
    R3N_DEFINE_KEY(GRAVE, 0xC0),
    R3N_MAX_KEYS = 256
} R3N_Keyboard_Key;

typedef struct R3N_Inputs {
    R3N_FNPTR(none, process_mouse_wheel_input, s8 z);
    R3N_FNPTR(none, process_mouse_move_input, s16 x, s16 y);
    R3N_FNPTR(none, process_key_input, R3N_Keyboard_Key key, u8 pressed);
    R3N_FNPTR(none, process_mouse_button_input, R3N_Mouse_Button button, u8 pressed);

    R3N_FNPTR(none, reset, none);
    R3N_FNPTR(none, update, none);

    R3N_FNPTR(u8, key_is_up, R3N_Keyboard_Key key);
    R3N_FNPTR(u8, key_was_up, R3N_Keyboard_Key key);
    R3N_FNPTR(u8, key_is_down, R3N_Keyboard_Key key);
    R3N_FNPTR(u8, key_was_down, R3N_Keyboard_Key key);

    R3N_FNPTR(u8, button_is_up, R3N_Mouse_Button button);
    R3N_FNPTR(u8, button_was_up, R3N_Mouse_Button button);
    R3N_FNPTR(u8, button_is_down, R3N_Mouse_Button button);
    R3N_FNPTR(u8, button_was_down, R3N_Mouse_Button button);

    R3N_FNPTR(none, mouse_get_position, s16* x, s16* y);
    R3N_FNPTR(none, mouse_get_last_position, s16* x, s16* y);
} R3N_Inputs;
R3N_API R3N_Inputs* R3N_Inputs_Ptr;

R3N_API u8 r3n_init_inputs(none);
R3N_API u8 r3n_deinit_inputs(none);

#endif // __R3N_INPUTS_H__