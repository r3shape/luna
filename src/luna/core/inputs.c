#include <include/luna/core/inputs.h>
#include <include/luna/core/events.h>

static struct LunaInputsInternal {
    struct devices {
        u8 mouseButtons[LUNA_MOUSE_MAX_BUTTONS];
        u8 keyboard[LUNA_MAX_KEYS];
        i16 mouseDelta[2];
    } devices[2];
} LunaInputsInternal = {0};

// global dispatch table ptr
LunaInputs* lunaInputs = NULL;

// internal dispatch table ptr
static LunaEvents* inputEvents = NULL;

void updateImpl(void) {
    memcpy(&LunaInputsInternal.devices[0], &LunaInputsInternal.devices[1], sizeof(LunaInputsInternal.devices[1]));
    
    // reset current rawinput mouse deltas
    LunaInputsInternal.devices[1].mouseDelta[0] = 0.0;
    LunaInputsInternal.devices[1].mouseDelta[1] = 0.0;
}

void resetImpl(void) {
    SSDK_FORI(0, 2, 1) {
        LunaInputsInternal.devices[i].mouseDelta[0] = 0;
        LunaInputsInternal.devices[i].mouseDelta[1] = 0;
    }
    
    SSDK_FORI(0, 2, 1) {
        SSDK_FORJ(0, 256, 1) {
            LunaInputsInternal.devices[i].keyboard[j] = 0;
        }
    }
    
    SSDK_FORI(0, 2, 1) {
        SSDK_FORJ(0, LUNA_MOUSE_MAX_BUTTONS, 1) {
            LunaInputsInternal.devices[i].mouseButtons[j] = 0;
        }
    }
}

u8 keyIsUpImpl(LunaKeyboardKey key) {
    return LunaInputsInternal.devices[1].keyboard[key] == SSDK_FALSE;
}

u8 keyWasUpImpl(LunaKeyboardKey key) {
    return LunaInputsInternal.devices[0].keyboard[key] == SSDK_FALSE;
}

u8 keyIsDownImpl(LunaKeyboardKey key) {
    return LunaInputsInternal.devices[1].keyboard[key] == SSDK_TRUE;
}

u8 keyWasDownImpl(LunaKeyboardKey key) {
    return LunaInputsInternal.devices[0].keyboard[key] == SSDK_TRUE;
}

u8 buttonIsUpImpl(LunaMouseButton button) {
    return LunaInputsInternal.devices[1].mouseButtons[button] == SSDK_FALSE;
}

u8 buttonWasUpImpl(LunaMouseButton button) {
    return LunaInputsInternal.devices[0].mouseButtons[button] == SSDK_FALSE;
}

u8 buttonIsDownImpl(LunaMouseButton button) {
    return LunaInputsInternal.devices[1].mouseButtons[button] == SSDK_TRUE;
}

u8 buttonWasDownImpl(LunaMouseButton button) {
    return LunaInputsInternal.devices[0].mouseButtons[button] == SSDK_TRUE;
}

void mouseGetPositionImpl(i16* x, i16* y) {
    *x = LunaInputsInternal.devices[1].mouseDelta[0];
    *y = LunaInputsInternal.devices[1].mouseDelta[1];
}

void mouseGetLastPositionImpl(i16* x, i16* y) {
    *x = LunaInputsInternal.devices[0].mouseDelta[0];
    *y = LunaInputsInternal.devices[0].mouseDelta[1];
}

void processMouseWheelInputImpl(i8 z_delta) {
    inputEvents->pushEvent(LUNA_EVENT_MOUSE_WHEEL, (LunaEvent){ .i8[0] = z_delta });
}

void processMouseMoveInputImpl(i16 x, i16 y) {
    if (x != 0 || y != 0) {
        LunaInputsInternal.devices[0].mouseDelta[0] = LunaInputsInternal.devices[1].mouseDelta[0];
        LunaInputsInternal.devices[0].mouseDelta[1] = LunaInputsInternal.devices[1].mouseDelta[1];

        LunaInputsInternal.devices[1].mouseDelta[0] += x; // WM_INPUT returns mouse deltas so just accumulate them
        LunaInputsInternal.devices[1].mouseDelta[1] += y;

        inputEvents->pushEvent(LUNA_EVENT_MOUSE_MOVE, (LunaEvent){
            .u16[0] = LunaInputsInternal.devices[1].mouseDelta[0],
            .u16[1] = LunaInputsInternal.devices[1].mouseDelta[1]
        });
    }
}

void processKeyInputImpl(LunaKeyboardKey key, u8 pressed) {
    if (LunaInputsInternal.devices[1].keyboard[key] != pressed) {
        LunaInputsInternal.devices[1].keyboard[key] = pressed;
        inputEvents->pushEvent(pressed ? LUNA_EVENT_KEY_PRESSED : LUNA_EVENT_KEY_RELEASED, (LunaEvent){ .u16[0] = key });
    }
}

void processMouseButtonInputImpl(LunaMouseButton button, u8 pressed) {
    if (LunaInputsInternal.devices[1].mouseButtons[button] != pressed) {
        LunaInputsInternal.devices[1].mouseButtons[button] = pressed;
        inputEvents->pushEvent(pressed ? LUNA_EVENT_BUTTON_PRESSED : LUNA_EVENT_BUTTON_RELEASED, (LunaEvent){ .u16[0] = button });
    }
}


byte lunaInitInputs(LunaInputs* table, ptr events_table) {
    if (!table || !events_table) {
        saneLog->log(SANE_LOG_ERROR, "[LunaInputs] invalid ptr :: lunaInitInputs()");
        return SSDK_FALSE;
    }

    // assign internal dispatch table ptr
    inputEvents = (LunaEvents*)events_table;

    table->reset = resetImpl;
    table->update = updateImpl;
    
    table->keyIsUp = keyIsUpImpl;
    table->keyWasUp = keyWasUpImpl;
    table->keyIsDown = keyIsDownImpl;
    table->keyWasDown = keyWasDownImpl;
    
    table->buttonIsUp = buttonIsUpImpl;
    table->buttonWasUp = buttonWasUpImpl;
    table->buttonIsDown = buttonIsDownImpl;
    table->buttonWasDown = buttonWasDownImpl;
    table->mouseGetPosition = mouseGetPositionImpl;
    table->mouseGetLastPosition = mouseGetLastPositionImpl;

    table->processKeyInput = processKeyInputImpl;
    table->processMouseMoveInput = processMouseMoveInputImpl;
    table->processMouseWheelInput = processMouseWheelInputImpl;
    table->processMouseButtonInput = processMouseButtonInputImpl;

    // assign global dispatch table ptr
    lunaInputs = table;

    saneLog->log(SANE_LOG_SUCCESS, "[LunaInputs] Initialized");
    
    return SSDK_TRUE;
}

byte lunaDeinitInputs(LunaInputs* table) {
    if (!table) {
        saneLog->log(SANE_LOG_ERROR, "[LunaInputs] invalid ptr :: lunaDeinitInputs()");
        return SSDK_FALSE;
    }

    // null global dispatch table ptr
    lunaInputs = NULL;

    table->reset = NULL;
    table->update = NULL;
    
    table->keyIsUp = NULL;
    table->keyWasUp = NULL;
    table->keyIsDown = NULL;
    table->keyWasDown = NULL;
    
    table->buttonIsUp = NULL;
    table->buttonWasUp = NULL;
    table->buttonIsDown = NULL;
    table->buttonWasDown = NULL;
    table->mouseGetPosition = NULL;
    table->mouseGetLastPosition = NULL;

    table->processKeyInput = NULL;
    table->processMouseMoveInput = NULL;
    table->processMouseWheelInput = NULL;
    table->processMouseButtonInput = NULL;

    saneLog->log(SANE_LOG_SUCCESS, "[LunaInputs] table deinitialized");

    return SSDK_TRUE;
}

