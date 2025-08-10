#ifndef __R3N_H__
#define __R3N_H__

#include <r3ngin/include/r3n.core/events.h>
#include <r3ngin/include/r3n.core/inputs.h>
#include <r3ngin/include/r3n.core/renderer.h>

#define R3N_USER_PATH_DEFAULT "r3n.data/user"

typedef struct R3N_Runtime_Config {
    str title;
    Vec2 windowPos;
    Vec2 windowSize;
    u32 backend;
} R3N_Runtime_Config;

typedef R3N_FNPTR(none, R3N_Runtime_Init_Func, none);
typedef R3N_FNPTR(none, R3N_Runtime_Update_Func, f32);
typedef R3N_FNPTR(none, R3N_Runtime_Render_Func, none);
typedef R3N_FNPTR(none, R3N_Runtime_Deinit_Func, none);
typedef R3N_FNPTR(none, R3N_Runtime_Config_Func, R3N_Runtime_Config);

typedef struct R3N_Runtime {
    R3N_Runtime_Init_Func init;
    R3N_Runtime_Update_Func update;
    R3N_Runtime_Render_Func render;
    R3N_Runtime_Deinit_Func deinit;
    R3N_Runtime_Config_Func configure;
} R3N_Runtime;

typedef R3N_FNPTR(none, R3N_Runtime_Export_Func, R3N_Runtime*);

#endif // __R3N_H__