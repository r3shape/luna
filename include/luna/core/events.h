#ifndef __LUNA_EVENTS_H__
#define __LUNA_EVENTS_H__

#define _LUNA_INTERNAL_
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
} LunaEvent;
typedef u8 (*LunaEventCallback)(LunaEventCode code, LunaEvent event);

typedef struct LunaEventApi {
    u8 (*registerEvent)(LunaEventCode code);
    u8 (*unregisterEvent)(LunaEventCode code);
    u8 (*pushEvent)(LunaEventCode code, LunaEvent event);

    u8 (*registerCallback)(LunaEventCode code, LunaEventCallback callback);
    u8 (*unregisterCallback)(LunaEventCode code, LunaEventCallback callback);
} LunaEventApi;
LUNA_API LunaEventApi* lunaEventApi;

LUNA_API u8 lunaInitEvents(none);
LUNA_API u8 lunaDeinitEvents(none);

#endif // __LUNA_EVENTS_H__