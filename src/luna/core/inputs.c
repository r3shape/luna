#include <include/luna/core/inputs.h>
#include <include/luna/core/events.h>
#include <include/r3kt/mem/alloc.h>
#include <include/r3kt/io/log.h>

static struct LunaInputsInternal {
    struct devices {
        u8 mouseButtons[LUNA_MOUSE_MAX_BUTTONS];
        u8 keyboard[LUNA_MAX_KEYS];
        s16 mouseDelta[2];
    } devices[2];
} LunaInputsInternal = {0};

// global dispatch table ptr
LunaInputApi* lunaInputApi = NULL;

void updateImpl(void) {
    r3_mem_write(sizeof(LunaInputsInternal.devices[1]), &LunaInputsInternal.devices[1], &LunaInputsInternal.devices[0]);
    
    // reset current rawinput mouse deltas
    LunaInputsInternal.devices[1].mouseDelta[0] = 0.0;
    LunaInputsInternal.devices[1].mouseDelta[1] = 0.0;
}

void resetImpl(void) {
    FOR_I(0, 2, 1) {
        LunaInputsInternal.devices[i].mouseDelta[0] = 0;
        LunaInputsInternal.devices[i].mouseDelta[1] = 0;
    }
    
    FOR_I(0, 2, 1) {
        FOR_J(0, 256, 1) {
            LunaInputsInternal.devices[i].keyboard[j] = 0;
        }
    }
    
    FOR_I(0, 2, 1) {
        FOR_J(0, LUNA_MOUSE_MAX_BUTTONS, 1) {
            LunaInputsInternal.devices[i].mouseButtons[j] = 0;
        }
    }
}

u8 keyIsUpImpl(LunaKeyboardKey key) {
    return LunaInputsInternal.devices[1].keyboard[key] == 0;
}

u8 keyWasUpImpl(LunaKeyboardKey key) {
    return LunaInputsInternal.devices[0].keyboard[key] == 0;
}

u8 keyIsDownImpl(LunaKeyboardKey key) {
    return LunaInputsInternal.devices[1].keyboard[key] == 1;
}

u8 keyWasDownImpl(LunaKeyboardKey key) {
    return LunaInputsInternal.devices[0].keyboard[key] == 1;
}

u8 buttonIsUpImpl(LunaMouseButton button) {
    return LunaInputsInternal.devices[1].mouseButtons[button] == 0;
}

u8 buttonWasUpImpl(LunaMouseButton button) {
    return LunaInputsInternal.devices[0].mouseButtons[button] == 0;
}

u8 buttonIsDownImpl(LunaMouseButton button) {
    return LunaInputsInternal.devices[1].mouseButtons[button] == 1;
}

u8 buttonWasDownImpl(LunaMouseButton button) {
    return LunaInputsInternal.devices[0].mouseButtons[button] == 1;
}

void mouseGetPositionImpl(s16* x, s16* y) {
    *x = LunaInputsInternal.devices[1].mouseDelta[0];
    *y = LunaInputsInternal.devices[1].mouseDelta[1];
}

void mouseGetLastPositionImpl(s16* x, s16* y) {
    *x = LunaInputsInternal.devices[0].mouseDelta[0];
    *y = LunaInputsInternal.devices[0].mouseDelta[1];
}

void processMouseWheelInputImpl(s8 z_delta) {
    lunaEventApi->pushEvent(LUNA_EVENT_MOUSE_WHEEL, (LunaEvent){ .s8[0] = z_delta });
}

void processMouseMoveInputImpl(s16 x, s16 y) {
    if (x != 0 || y != 0) {
        LunaInputsInternal.devices[0].mouseDelta[0] = LunaInputsInternal.devices[1].mouseDelta[0];
        LunaInputsInternal.devices[0].mouseDelta[1] = LunaInputsInternal.devices[1].mouseDelta[1];

        LunaInputsInternal.devices[1].mouseDelta[0] += x; // WM_INPUT returns mouse deltas so just accumulate them
        LunaInputsInternal.devices[1].mouseDelta[1] += y;

        lunaEventApi->pushEvent(LUNA_EVENT_MOUSE_MOVE, (LunaEvent){
            .u16[0] = LunaInputsInternal.devices[1].mouseDelta[0],
            .u16[1] = LunaInputsInternal.devices[1].mouseDelta[1]
        });
    }
}

void processKeyInputImpl(LunaKeyboardKey key, u8 pressed) {
    if (LunaInputsInternal.devices[1].keyboard[key] != pressed) {
        LunaInputsInternal.devices[1].keyboard[key] = pressed;
        lunaEventApi->pushEvent(pressed ? LUNA_EVENT_KEY_PRESSED : LUNA_EVENT_KEY_RELEASED, (LunaEvent){ .u16[0] = key });
    }
}

void processMouseButtonInputImpl(LunaMouseButton button, u8 pressed) {
    if (LunaInputsInternal.devices[1].mouseButtons[button] != pressed) {
        LunaInputsInternal.devices[1].mouseButtons[button] = pressed;
        lunaEventApi->pushEvent(pressed ? LUNA_EVENT_BUTTON_PRESSED : LUNA_EVENT_BUTTON_RELEASED, (LunaEvent){ .u16[0] = button });
    }
}


u8 lunaInitInputs(none) {
    if (lunaEventApi == NULL) {
        r3_log_stdout(ERROR_LOG, "[LunaInputs] LunaEventApi not initialized!\n");
        return 0;
    }

    if (lunaInputApi == NULL) {
        lunaInputApi = r3_mem_alloc(sizeof(LunaInputApi), 8);
        if (lunaInputApi == NULL) {
            r3_log_stdout(ERROR_LOG, "[LunaInputs] failed to allocate lunaInputApi dispatch table!\n");
            return 0;
        }
    }

    lunaInputApi->reset = resetImpl;
    lunaInputApi->update = updateImpl;
    
    lunaInputApi->keyIsUp = keyIsUpImpl;
    lunaInputApi->keyWasUp = keyWasUpImpl;
    lunaInputApi->keyIsDown = keyIsDownImpl;
    lunaInputApi->keyWasDown = keyWasDownImpl;
    
    lunaInputApi->buttonIsUp = buttonIsUpImpl;
    lunaInputApi->buttonWasUp = buttonWasUpImpl;
    lunaInputApi->buttonIsDown = buttonIsDownImpl;
    lunaInputApi->buttonWasDown = buttonWasDownImpl;
    lunaInputApi->mouseGetPosition = mouseGetPositionImpl;
    lunaInputApi->mouseGetLastPosition = mouseGetLastPositionImpl;

    lunaInputApi->processKeyInput = processKeyInputImpl;
    lunaInputApi->processMouseMoveInput = processMouseMoveInputImpl;
    lunaInputApi->processMouseWheelInput = processMouseWheelInputImpl;
    lunaInputApi->processMouseButtonInput = processMouseButtonInputImpl;

    r3_log_stdout(SUCCESS_LOG, "[LunaEvents] initialized lunaInputApi\n");
    return 1;
}

u8 lunaDeinitInputs(none) {
    if (lunaInputApi != NULL) {
        r3_mem_dealloc(lunaInputApi);
        lunaInputApi = NULL;
    }

    r3_log_stdout(SUCCESS_LOG, "[LunaEvents] deinitialized lunaInputApi\n");
    return 1;
}
