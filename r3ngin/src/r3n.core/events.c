#include <r3ngin/include/r3n.core/events.h>
#include <r3kit/include/io/log.h>
#include <r3kit/include/ds/arr.h>

static struct R3N_Events_Internal {
    u16 events;
    u8 eventv[R3N_EVENT_CODE_MAX];
    Array callbackv[R3N_EVENT_CODE_MAX];
} R3N_Events_Internal = {0};

// global dispatch table ptr
R3N_Events* R3N_Events_Ptr = NULL;


u8 register_event_impl(R3N_Event_Code eventCode) {
    if (eventCode >= R3N_EVENT_CODE_MAX || R3N_Events_Internal.eventv[eventCode]) 
        return 0;

    R3N_Events_Internal.eventv[eventCode] = 1;
    if (!r3_arr_alloc(R3N_EVENT_CALLBACK_MAX, sizeof(R3N_Event_Hook), &R3N_Events_Internal.callbackv[eventCode])) {
        r3_log_stdoutf(ERROR_LOG, "[LunaEvents] failed to register event: %d\n", eventCode);
        return 0;
    }

    R3N_Events_Internal.events++;
    return 1;
}

u8 unregister_event_impl(R3N_Event_Code eventCode) {
    if (eventCode >= R3N_EVENT_CODE_MAX || !R3N_Events_Internal.eventv[eventCode]) 
        return 0;

    R3N_Events_Internal.eventv[eventCode] = 0;

    if (R3N_Events_Internal.callbackv[eventCode].data) {

        if (!r3_arr_dealloc(&R3N_Events_Internal.callbackv[eventCode])) {
            r3_log_stdoutf(ERROR_LOG, "[LunaEvents] failed to deallocate event callback: %d\n", eventCode);
        } else {
            R3N_Events_Internal.callbackv[eventCode] = (Array){NULL};
        }
    }

    R3N_Events_Internal.events--;
    return 1;
}

u8 push_event_impl(R3N_Event_Code eventCode, R3N_Event data) {
    if (eventCode >= R3N_EVENT_CODE_MAX || !R3N_Events_Internal.eventv[eventCode]) 
        return 0;

    if (!R3N_Events_Internal.callbackv[eventCode].data) return 0;

    u8 result = 0;
    u16 count = r3_arr_field(ARRAY_COUNT_FIELD, &R3N_Events_Internal.callbackv[eventCode]);
	r3_log_stdoutf(INFO_LOG, "[LunaEvents] event code callback count: (code)%d (count)%d\n", eventCode, count);
    FOR_I(0, count, 1) {
        R3N_Event_Hook callback = {0};
        if (!r3_arr_read(i, &callback, &R3N_Events_Internal.callbackv[eventCode]) || !callback) {
            r3_log_stdoutf(WARN_LOG, "[LunaEvents] failed to read event callback: %d\n", eventCode);
        } else {
            result = callback(eventCode, data);
        }
    }

    return result;
}

u8 register_callback_impl(R3N_Event_Code eventCode, R3N_Event_Hook callback) {
    if (eventCode >= R3N_EVENT_CODE_MAX || !R3N_Events_Internal.eventv[eventCode]) 
        return 0;

    if (!R3N_Events_Internal.callbackv[eventCode].data) 
        return 0;

    if (!r3_arr_assign(r3_arr_field(ARRAY_COUNT_FIELD, &R3N_Events_Internal.callbackv[eventCode]), callback, &R3N_Events_Internal.callbackv[eventCode])) {
		r3_log_stdoutf(ERROR_LOG, "[LunaEvents] failed to register event callback: (code)%d\n", eventCode);
		return 0;
	}
    return 1;
}

u8 unregister_callback_impl(R3N_Event_Code eventCode, R3N_Event_Hook callback) {
    if (eventCode >= R3N_EVENT_CODE_MAX || !R3N_Events_Internal.eventv[eventCode]) {
        return 0;
    }

    u16 count = r3_arr_field(ARRAY_COUNT_FIELD, &R3N_Events_Internal.callbackv[eventCode]);
    if (count == 0) return 0;

    FOR_I(0, count, 1) {
        R3N_Event_Hook readback = {0};
        if (((R3N_Event_Hook*)R3N_Events_Internal.callbackv[eventCode].data)[count] == callback) {
            r3_arr_pull(count, &readback, &R3N_Events_Internal.callbackv[eventCode]);
            return 1;
        }
    }
    return 0;
}


u8 r3n_init_events(none) {
    if (R3N_Events_Ptr == NULL) {
        R3N_Events_Ptr = r3_mem_alloc(sizeof(R3N_Events), 8);
        if (R3N_Events_Ptr == NULL) {
            r3_log_stdout(ERROR_LOG, "[LunaEvents] failed to allocate R3N_Events dispatch table!\n");
            return 0;
        } 
    }

    R3N_Events_Ptr->push_event = push_event_impl;
    R3N_Events_Ptr->register_event = register_event_impl;
    R3N_Events_Ptr->unregister_event = unregister_event_impl;

    R3N_Events_Ptr->register_callback = register_callback_impl;
    R3N_Events_Ptr->unregister_callback = unregister_callback_impl;

    FOR_I(0, R3N_EVENT_CODES, 1) {
        if (!R3N_Events_Ptr->register_event(i)) {
            r3_log_stdoutf(ERROR_LOG, "[LunaEvents] failed to register: %u\n", i);
        } else r3_log_stdoutf(SUCCESS_LOG, "[LunaEvents] registered: %u\n", i);
    }

    r3_log_stdout(SUCCESS_LOG, "[LunaEvents] initialized R3N_Events\n");
    return 1;
}

u8 r3n_deinit_events(none) {
    if (R3N_Events_Ptr != NULL) {
        FOR_I(0, R3N_EVENT_CODE_MAX, 1) {
            if (R3N_Events_Internal.eventv[i]) {
                R3N_Events_Ptr->unregister_event(i);
            }
        }
        r3_mem_dealloc(R3N_Events_Ptr);
        R3N_Events_Ptr = NULL;
    }

    r3_log_stdout(SUCCESS_LOG, "[LunaEvents] deinitialized R3N_Events\n");
    return 1;
}
