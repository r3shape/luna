#ifndef __R3N_EVENTS_H__
#define __R3N_EVENTS_H__

#define _R3KIT_INCLUDE_
#include <r3ngin/include/r3n.core/defines.h>

#define R3N_EVENT_CALLBACK_MAX     (1U << 5)
#define R3N_EVENT_MAX              ((1U << 12) - 1)

typedef enum R3N_Event_Code {
    R3N_EVENT_EXIT,
    R3N_EVENT_KEY_PRESSED,
    R3N_EVENT_KEY_RELEASED,
    R3N_EVENT_BUTTON_PRESSED,
    R3N_EVENT_BUTTON_RELEASED,
    R3N_EVENT_MOUSE_MOVE,
    R3N_EVENT_MOUSE_WHEEL,
    R3N_EVENT_RESIZE,
    R3N_EVENT_CODES,
    R3N_EVENT_CODE_MAX = ((1U << 8) - 1)
} R3N_Event_Code;

typedef union R3N_Event {
    // 128 byte maximum
    s64 s64[2];
    u64 u64[2];
    f64 f64[2];

    s32 s32[4];
    u32 u32[4];
    f32 f32[4];

    s16 s16[8];
    u16 u16[8];
    
    s8 s8[16];
    u8 u8[16];
    char c[16];
} R3N_Event;
typedef u8 (*R3N_Event_Hook)(R3N_Event_Code code, R3N_Event event);

typedef struct R3N_Events {
    R3N_FNPTR(u8, register_event, R3N_Event_Code code);
    R3N_FNPTR(u8, unregister_event, R3N_Event_Code code);
    R3N_FNPTR(u8, push_event, R3N_Event_Code code, R3N_Event event);

    R3N_FNPTR(u8, register_callback, R3N_Event_Code code, R3N_Event_Hook callback);
    R3N_FNPTR(u8, unregister_callback, R3N_Event_Code code, R3N_Event_Hook callback);
} R3N_Events;
R3N_API R3N_Events* R3N_Events_Ptr;

R3N_API u8 r3n_init_events(none);
R3N_API u8 r3n_deinit_events(none);

#endif // __R3N_EVENTS_H__