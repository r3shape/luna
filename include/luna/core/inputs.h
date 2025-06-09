#ifndef __LUNA_INPUTS_H__
#define __LUNA_INPUTS_H__

#include <include/SSDK/SSDK.h>
#include <include/luna/core/defines.h>

#define LUNA_DEFINE_KEY(name, code) LUNA_KEY_##name = code

typedef enum LunaMouseButton {
    LUNA_MBUTTON_LEFT,
    LUNA_MBUTTON_RIGHT,
    LUNA_MBUTTON_MIDDLE,
    LUNA_MBUTTON_SIDE1,
    LUNA_MBUTTON_SIDE2,
    LUNA_MOUSE_MAX_BUTTONS
} LunaMouseButton;

typedef enum LunaKeyboardKey {
    LUNA_DEFINE_KEY(BACKSPACE, 0x08),
    LUNA_DEFINE_KEY(ENTER, 0x0D),
    LUNA_DEFINE_KEY(TAB, 0x09),
    LUNA_DEFINE_KEY(SHIFT, 0x10),
    LUNA_DEFINE_KEY(CONTROL, 0x11),
    LUNA_DEFINE_KEY(PAUSE, 0x13),
    LUNA_DEFINE_KEY(CAPITAL, 0x14),
    LUNA_DEFINE_KEY(ESCAPE, 0x1B),
    LUNA_DEFINE_KEY(CONVERT, 0x1C),
    LUNA_DEFINE_KEY(NONCONVERT, 0x1D),
    LUNA_DEFINE_KEY(ACCEPT, 0x1E),
    LUNA_DEFINE_KEY(MODECHANGE, 0x1F),
    LUNA_DEFINE_KEY(SPACE, 0x20),
    LUNA_DEFINE_KEY(PRIOR, 0x21),
    LUNA_DEFINE_KEY(NEXT, 0x22),
    LUNA_DEFINE_KEY(END, 0x23),
    LUNA_DEFINE_KEY(HOME, 0x24),
    LUNA_DEFINE_KEY(LEFT, 0x25),
    LUNA_DEFINE_KEY(UP, 0x26),
    LUNA_DEFINE_KEY(RIGHT, 0x27),
    LUNA_DEFINE_KEY(DOWN, 0x28),
    LUNA_DEFINE_KEY(SELECT, 0x29),
    LUNA_DEFINE_KEY(PRINT, 0x2A),
    LUNA_DEFINE_KEY(EXECUTE, 0x2B),
    LUNA_DEFINE_KEY(SNAPSHOT, 0x2C),
    LUNA_DEFINE_KEY(INSERT, 0x2D),
    LUNA_DEFINE_KEY(DELETE, 0x2E),
    LUNA_DEFINE_KEY(HELP, 0x2F),
    LUNA_DEFINE_KEY(A, 0x41),
    LUNA_DEFINE_KEY(B, 0x42),
    LUNA_DEFINE_KEY(C, 0x43),
    LUNA_DEFINE_KEY(D, 0x44),
    LUNA_DEFINE_KEY(E, 0x45),
    LUNA_DEFINE_KEY(F, 0x46),
    LUNA_DEFINE_KEY(G, 0x47),
    LUNA_DEFINE_KEY(H, 0x48),
    LUNA_DEFINE_KEY(I, 0x49),
    LUNA_DEFINE_KEY(J, 0x4A),
    LUNA_DEFINE_KEY(K, 0x4B),
    LUNA_DEFINE_KEY(L, 0x4C),
    LUNA_DEFINE_KEY(M, 0x4D),
    LUNA_DEFINE_KEY(N, 0x4E),
    LUNA_DEFINE_KEY(O, 0x4F),
    LUNA_DEFINE_KEY(P, 0x50),
    LUNA_DEFINE_KEY(Q, 0x51),
    LUNA_DEFINE_KEY(R, 0x52),
    LUNA_DEFINE_KEY(S, 0x53),
    LUNA_DEFINE_KEY(T, 0x54),
    LUNA_DEFINE_KEY(U, 0x55),
    LUNA_DEFINE_KEY(V, 0x56),
    LUNA_DEFINE_KEY(W, 0x57),
    LUNA_DEFINE_KEY(X, 0x58),
    LUNA_DEFINE_KEY(Y, 0x59),
    LUNA_DEFINE_KEY(Z, 0x5A),
    LUNA_DEFINE_KEY(LWIN, 0x5B),
    LUNA_DEFINE_KEY(RWIN, 0x5C),
    LUNA_DEFINE_KEY(APPS, 0x5D),
    LUNA_DEFINE_KEY(SLEEP, 0x5F),
    LUNA_DEFINE_KEY(NUMPAD0, 0x60),    // wrong on win32????
    LUNA_DEFINE_KEY(NUMPAD1, 0x61),    // wrong on win32????
    LUNA_DEFINE_KEY(NUMPAD2, 0x62),    // wrong on win32????
    LUNA_DEFINE_KEY(NUMPAD3, 0x63),    // wrong on win32????
    LUNA_DEFINE_KEY(NUMPAD4, 0x64),    // wrong on win32????
    LUNA_DEFINE_KEY(NUMPAD5, 0x65),    // wrong on win32????
    LUNA_DEFINE_KEY(NUMPAD6, 0x66),    // wrong on win32????
    LUNA_DEFINE_KEY(NUMPAD7, 0x67),    // wrong on win32????
    LUNA_DEFINE_KEY(NUMPAD8, 0x68),    // wrong on win32????
    LUNA_DEFINE_KEY(NUMPAD9, 0x69),    // wrong on win32????
    LUNA_DEFINE_KEY(MULTIPLY, 0x6A),
    LUNA_DEFINE_KEY(ADD, 0x6B),
    LUNA_DEFINE_KEY(SEPARATOR, 0x6C),
    LUNA_DEFINE_KEY(SUBTRACT, 0x6D),
    LUNA_DEFINE_KEY(DECIMAL, 0x6E),
    LUNA_DEFINE_KEY(DIVIDE, 0x6F),
    LUNA_DEFINE_KEY(F1, 0x70),
    LUNA_DEFINE_KEY(F2, 0x71),
    LUNA_DEFINE_KEY(F3, 0x72),
    LUNA_DEFINE_KEY(F4, 0x73),
    LUNA_DEFINE_KEY(F5, 0x74),
    LUNA_DEFINE_KEY(F6, 0x75),
    LUNA_DEFINE_KEY(F7, 0x76),
    LUNA_DEFINE_KEY(F8, 0x77),
    LUNA_DEFINE_KEY(F9, 0x78),
    LUNA_DEFINE_KEY(F10, 0x79),
    LUNA_DEFINE_KEY(F11, 0x7A),
    LUNA_DEFINE_KEY(F12, 0x7B),
    LUNA_DEFINE_KEY(F13, 0x7C),
    LUNA_DEFINE_KEY(F14, 0x7D),
    LUNA_DEFINE_KEY(F15, 0x7E),
    LUNA_DEFINE_KEY(F16, 0x7F),
    LUNA_DEFINE_KEY(F17, 0x80),
    LUNA_DEFINE_KEY(F18, 0x81),
    LUNA_DEFINE_KEY(F19, 0x82),
    LUNA_DEFINE_KEY(F20, 0x83),
    LUNA_DEFINE_KEY(F21, 0x84),
    LUNA_DEFINE_KEY(F22, 0x85),
    LUNA_DEFINE_KEY(F23, 0x86),
    LUNA_DEFINE_KEY(F24, 0x87),
    LUNA_DEFINE_KEY(NUMLOCK, 0x90),
    LUNA_DEFINE_KEY(SCROLL, 0x91),
    LUNA_DEFINE_KEY(NUMPAD_EQUAL, 0x92),
    // LUNA_DEFINE_KEY(LSHIFT, 0xA0),
    // LUNA_DEFINE_KEY(RSHIFT, 0xA1),
    LUNA_DEFINE_KEY(LCONTROL, 0xA2),
    LUNA_DEFINE_KEY(RCONTROL, 0xA3),
    LUNA_DEFINE_KEY(LMENU, 0xA4),
    LUNA_DEFINE_KEY(RMENU, 0xA5),
    LUNA_DEFINE_KEY(SEMICOLON, 0xBA),
    LUNA_DEFINE_KEY(PLUS, 0xBB),
    LUNA_DEFINE_KEY(COMMA, 0xBC),
    LUNA_DEFINE_KEY(MINUS, 0xBD),
    LUNA_DEFINE_KEY(PERIOD, 0xBE),
    LUNA_DEFINE_KEY(SLASH, 0xBF),
    LUNA_DEFINE_KEY(GRAVE, 0xC0),
    LUNA_MAX_KEYS = 256
} LunaKeyboardKey;

typedef struct LunaInputs {
    none (*processMouseWheelInput)(i8 z);
    none (*processMouseMoveInput)(i16 x, i16 y);
    none (*processKeyInput)(LunaKeyboardKey key, u8 pressed);
    none (*processMouseButtonInput)(LunaMouseButton button, u8 pressed);

    none (*reset)(none);
    none (*update)(none);

    u8 (*keyIsUp)(LunaKeyboardKey key);
    u8 (*keyWasUp)(LunaKeyboardKey key);
    u8 (*keyIsDown)(LunaKeyboardKey key);
    u8 (*keyWasDown)(LunaKeyboardKey key);

    u8 (*buttonIsUp)(LunaMouseButton button);
    u8 (*buttonWasUp)(LunaMouseButton button);
    u8 (*buttonIsDown)(LunaMouseButton button);
    u8 (*buttonWasDown)(LunaMouseButton button);

    none (*mouseGetPosition)(i16* x, i16* y);
    none (*mouseGetLastPosition)(i16* x, i16* y);
} LunaInputs;

LUNA_API byte lunaInitInputs(LunaInputs* table, ptr events_table);
LUNA_API byte lunaDeinitInputs(LunaInputs* table);

#endif // __LUNA_INPUTS_H__