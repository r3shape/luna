#include <r3ngin/include/r3n.core/inputs.h>
#include <r3ngin/include/r3n.core/events.h>
#include <r3kit/include/mem/alloc.h>
#include <r3kit/include/io/log.h>

static struct R3N_Inputs_Internal {
    struct devices {
        u8 mouse_buttons[R3N_MOUSE_MAX_BUTTONS];
        u8 keyboard[R3N_MAX_KEYS];
        s16 mouse_delta[2];
    } devices[2];
} R3N_Inputs_Internal = {0};

// global dispatch table ptr
R3N_Inputs* R3N_Inputs_Ptr = NULL;

void update_impl(void) {
    r3_mem_write(sizeof(R3N_Inputs_Internal.devices[1]), &R3N_Inputs_Internal.devices[1], &R3N_Inputs_Internal.devices[0]);
    
    // reset current rawinput mouse deltas
    R3N_Inputs_Internal.devices[1].mouse_delta[0] = 0.0;
    R3N_Inputs_Internal.devices[1].mouse_delta[1] = 0.0;
}

void reset_impl(void) {
    FOR_I(0, 2, 1) {
        R3N_Inputs_Internal.devices[i].mouse_delta[0] = 0;
        R3N_Inputs_Internal.devices[i].mouse_delta[1] = 0;
    }
    
    FOR_I(0, 2, 1) {
        FOR_J(0, 256, 1) {
            R3N_Inputs_Internal.devices[i].keyboard[j] = 0;
        }
    }
    
    FOR_I(0, 2, 1) {
        FOR_J(0, R3N_MOUSE_MAX_BUTTONS, 1) {
            R3N_Inputs_Internal.devices[i].mouse_buttons[j] = 0;
        }
    }
}

u8 key_is_up_impl(R3N_Keyboard_Key key) {
    return R3N_Inputs_Internal.devices[1].keyboard[key] == 0;
}

u8 key_was_up_impl(R3N_Keyboard_Key key) {
    return R3N_Inputs_Internal.devices[0].keyboard[key] == 0;
}

u8 key_is_down_impl(R3N_Keyboard_Key key) {
    return R3N_Inputs_Internal.devices[1].keyboard[key] == 1;
}

u8 key_was_down_impl(R3N_Keyboard_Key key) {
    return R3N_Inputs_Internal.devices[0].keyboard[key] == 1;
}

u8 button_is_up_impl(R3N_Mouse_Button button) {
    return R3N_Inputs_Internal.devices[1].mouse_buttons[button] == 0;
}

u8 button_was_up_impl(R3N_Mouse_Button button) {
    return R3N_Inputs_Internal.devices[0].mouse_buttons[button] == 0;
}

u8 button_is_down_impl(R3N_Mouse_Button button) {
    return R3N_Inputs_Internal.devices[1].mouse_buttons[button] == 1;
}

u8 button_was_down_impl(R3N_Mouse_Button button) {
    return R3N_Inputs_Internal.devices[0].mouse_buttons[button] == 1;
}

void mouse_get_position_impl(s16* x, s16* y) {
    *x = R3N_Inputs_Internal.devices[1].mouse_delta[0];
    *y = R3N_Inputs_Internal.devices[1].mouse_delta[1];
}

void mouse_get_last_position_impl(s16* x, s16* y) {
    *x = R3N_Inputs_Internal.devices[0].mouse_delta[0];
    *y = R3N_Inputs_Internal.devices[0].mouse_delta[1];
}

void process_mouse_wheel_input_impl(s8 z_delta) {
    R3N_Events_Ptr->push_event(R3N_EVENT_MOUSE_WHEEL, (R3N_Event){ .s8[0] = z_delta });
}

void process_mouse_move_input_impl(s16 x, s16 y) {
    if (x != 0 || y != 0) {
        R3N_Inputs_Internal.devices[0].mouse_delta[0] = R3N_Inputs_Internal.devices[1].mouse_delta[0];
        R3N_Inputs_Internal.devices[0].mouse_delta[1] = R3N_Inputs_Internal.devices[1].mouse_delta[1];

        R3N_Inputs_Internal.devices[1].mouse_delta[0] += x; // WM_INPUT returns mouse deltas so just accumulate them
        R3N_Inputs_Internal.devices[1].mouse_delta[1] += y;

        R3N_Events_Ptr->push_event(R3N_EVENT_MOUSE_MOVE, (R3N_Event){
            .u16[0] = R3N_Inputs_Internal.devices[1].mouse_delta[0],
            .u16[1] = R3N_Inputs_Internal.devices[1].mouse_delta[1]
        });
    }
}

void process_key_input_impl(R3N_Keyboard_Key key, u8 pressed) {
    if (R3N_Inputs_Internal.devices[1].keyboard[key] != pressed) {
        R3N_Inputs_Internal.devices[1].keyboard[key] = pressed;
        R3N_Events_Ptr->push_event(pressed ? R3N_EVENT_KEY_PRESSED : R3N_EVENT_KEY_RELEASED, (R3N_Event){ .u16[0] = key });
    }
}

void process_mouse_button_input_impl(R3N_Mouse_Button button, u8 pressed) {
    if (R3N_Inputs_Internal.devices[1].mouse_buttons[button] != pressed) {
        R3N_Inputs_Internal.devices[1].mouse_buttons[button] = pressed;
        R3N_Events_Ptr->push_event(pressed ? R3N_EVENT_BUTTON_PRESSED : R3N_EVENT_BUTTON_RELEASED, (R3N_Event){ .u16[0] = button });
    }
}


u8 r3n_init_inputs(none) {
    if (R3N_Events_Ptr == NULL) {
        r3_log_stdout(ERROR_LOG, "[R3N_Inputs] R3N_Events not initialized!\n");
        return 0;
    }

    if (R3N_Inputs_Ptr == NULL) {
        R3N_Inputs_Ptr = r3_mem_alloc(sizeof(R3N_Inputs), 8);
        if (R3N_Inputs_Ptr == NULL) {
            r3_log_stdout(ERROR_LOG, "[R3N_Inputs] failed to allocate R3N_Inputs dispatch table!\n");
            return 0;
        }
    }

    R3N_Inputs_Ptr->reset = reset_impl;
    R3N_Inputs_Ptr->update = update_impl;
    
    R3N_Inputs_Ptr->key_is_up = key_is_up_impl;
    R3N_Inputs_Ptr->key_was_up = key_was_up_impl;
    R3N_Inputs_Ptr->key_is_down = key_is_down_impl;
    R3N_Inputs_Ptr->key_was_down = key_was_down_impl;
    
    R3N_Inputs_Ptr->button_is_up = button_is_up_impl;
    R3N_Inputs_Ptr->button_was_up = button_was_up_impl;
    R3N_Inputs_Ptr->button_is_down = button_is_down_impl;
    R3N_Inputs_Ptr->button_was_down = button_was_down_impl;
    R3N_Inputs_Ptr->mouse_get_position = mouse_get_position_impl;
    R3N_Inputs_Ptr->mouse_get_last_position = mouse_get_last_position_impl;

    R3N_Inputs_Ptr->process_key_input = process_key_input_impl;
    R3N_Inputs_Ptr->process_mouse_move_input = process_mouse_move_input_impl;
    R3N_Inputs_Ptr->process_mouse_wheel_input = process_mouse_wheel_input_impl;
    R3N_Inputs_Ptr->process_mouse_button_input = process_mouse_button_input_impl;

    r3_log_stdout(SUCCESS_LOG, "[R3N_Inputs] initialized R3N_Inputs\n");
    return 1;
}

u8 r3n_deinit_inputs(none) {
    if (R3N_Inputs_Ptr != NULL) {
        r3_mem_dealloc(R3N_Inputs_Ptr);
        R3N_Inputs_Ptr = NULL;
    }

    r3_log_stdout(SUCCESS_LOG, "[R3N_Inputs] deinitialized R3N_Inputs\n");
    return 1;
}
