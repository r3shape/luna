#ifndef __LUNA_EVENTS_H__
#define __LUNA_EVENTS_H__

#include <include/SSDK/SSDK.h>
#include <include/luna/core/defines.h>

#define LUNA_EVENT_CALLBACK_MAX     (1U << 5)
#define LUNA_EVENT_MAX              ((1U << 12) - 1)

typedef enum LunaEventCode {
    LUNA_EVENT_EXIT,
    LUNA_EVENT_KEY_PRESSED,
    LUNA_EVENT_KEY_RELEASED,
    LUNA_EVENT_BUTTON_PRESSED,
    LUNA_EVENT_BUTTON_RELEASED,
    LUNA_EVENT_MOUSE_MOVE,
    LUNA_EVENT_MOUSE_WHEEL,
    LUNA_EVENT_RESIZE,
    LUNA_EVENT_CODES,
    LUNA_EVENT_CODE_MAX = ((1U << 8) - 1)
} LunaEventCode;

typedef union LunaEvent {
    // 128 byte maximum
    i64 i64[2];
    u64 u64[2];
    f64 f64[2];

    i32 i32[4];
    u32 u32[4];
    f32 f32[4];

    i16 i16[8];
    u16 u16[8];
    
    i8 i8[16];
    u8 u8[16];
    char c[16];
} LunaEvent;
typedef u8 (*LunaEventCallback)(LunaEventCode code, LunaEvent event);

typedef struct LunaEvents {
    u8 (*registerEvent)(LunaEventCode code);
    u8 (*unregisterEvent)(LunaEventCode code);
    u8 (*pushEvent)(LunaEventCode code, LunaEvent event);

    u8 (*registerCallback)(LunaEventCode code, LunaEventCallback callback);
    u8 (*unregisterCallback)(LunaEventCode code, LunaEventCallback callback);
} LunaEvents;
extern LunaEvents* lunaEvents;

LUNA_API byte lunaInitEvents(LunaEvents* table);
LUNA_API byte lunaDeinitEvents(LunaEvents* table);

#endif // __LUNA_EVENTS_H__