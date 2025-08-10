#ifndef __R3N_PLATFORM_H__
#define __R3N_PLATFORM_H__

#define _R3KIT_INCLUDE_
#include <r3ngin/include/r3n.core/defines.h>

typedef enum R3N_Window_Flag {
    WINDOW_FOCUSED              = 1 << 0,
    WINDOW_SHOW_CURSOR          = 1 << 1,
    WINDOW_BIND_CURSOR          = 1 << 2,
    WINDOW_CENTER_CURSOR        = 1 << 3
} R3N_Window_Flag;

typedef struct R3N_Window {
    Vec2 location;
    Vec2 size;
    f32 aspect;
    u32 flags;
    str title;
} R3N_Window;

typedef struct R3N_Library {
    str name;
    ptr handle;
} R3N_Library;

typedef struct R3N_Platform {
    R3N_FNPTR(u8, create_window, str title, u32 width, u32 height, u32 x, u32 y, R3N_Window* window);
    R3N_FNPTR(none, destroy_window, none);

    R3N_FNPTR(none, poll_events, none);
    R3N_FNPTR(none, poll_inputs, none);

    R3N_FNPTR(none, swap_buffers, none);
    R3N_FNPTR(u8, create_gl_context, none);
    R3N_FNPTR(none, destroy_gl_context, none);

    R3N_FNPTR(u8, get_window_flag, u32 flag);
    R3N_FNPTR(u8, set_window_flag, u32 flag);
    R3N_FNPTR(u8, rem_window_flag, u32 flag);
    R3N_FNPTR(R3N_Window*, get_window, none);

    R3N_FNPTR(u8, unload_library, R3N_Library* library);
    R3N_FNPTR(u8, load_library, str path, str name, R3N_Library* library);
    R3N_FNPTR(u8, load_library_symbol, str name, ptr* symbol, R3N_Library* library);
} R3N_Platform;
R3N_API R3N_Platform* R3N_Platform_Ptr;

R3N_API u8 r3n_init_platform(none);
R3N_API u8 r3n_deinit_platform(none);

#endif // __R3N_PLATFORM_H__
