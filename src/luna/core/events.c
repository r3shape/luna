#include <include/luna/core/events.h>
#include <include/r3kt/io/log.h>
#include <include/r3kt/ds/arr.h>

static struct LunaEventsInternal {
    u16 events;
    u8 eventv[LUNA_EVENT_CODE_MAX];
    Array callbackv[LUNA_EVENT_CODE_MAX];
} LunaEventsInternal = {0};

// global dispatch table ptr
LunaEvents* lunaEvents = NULL;


u8 registerEventImpl(LunaEventCode eventCode) {
    if (eventCode >= LUNA_EVENT_CODE_MAX || LunaEventsInternal.eventv[eventCode]) 
        return 0;

    LunaEventsInternal.eventv[eventCode] = 1;
    if (!r3_arr_alloc(LUNA_EVENT_CALLBACK_MAX, sizeof(LunaEventCallback), &LunaEventsInternal.callbackv[eventCode])) {
        r3_log_stdoutf(ERROR_LOG, "[LunaEvents] failed to register event: %d\n", eventCode);
        return 0;
    }

    LunaEventsInternal.events++;
    return 1;
}

u8 unregisterEventImpl(LunaEventCode eventCode) {
    if (eventCode >= LUNA_EVENT_CODE_MAX || !LunaEventsInternal.eventv[eventCode]) 
        return 0;

    LunaEventsInternal.eventv[eventCode] = 0;

    if (LunaEventsInternal.callbackv[eventCode].data) {

        if (!r3_arr_dealloc(&LunaEventsInternal.callbackv[eventCode])) {
            r3_log_stdoutf(ERROR_LOG, "[LunaEvents] failed to deallocate event callback: %d\n", eventCode);
        } else {
            LunaEventsInternal.callbackv[eventCode] = (Array){NULL};
        }
    }

    LunaEventsInternal.events--;
    return 1;
}

u8 pushEventImpl(LunaEventCode eventCode, LunaEvent data) {
    if (eventCode >= LUNA_EVENT_CODE_MAX || !LunaEventsInternal.eventv[eventCode]) 
        return 0;

    if (!LunaEventsInternal.callbackv[eventCode].data) return 0;

    u8 result = 0;
    u16 count = r3_arr_field(ARRAY_COUNT_FIELD, &LunaEventsInternal.callbackv[eventCode]);
	r3_log_stdoutf(INFO_LOG, "[LunaEvents] event code callback count: (code)%d (count)%d\n", eventCode, count);
    FOR_I(0, count, 1) {
        LunaEventCallback callback = {0};
        if (!r3_arr_read(i, &callback, &LunaEventsInternal.callbackv[eventCode]) || !callback) {
            r3_log_stdoutf(WARN_LOG, "[LunaEvents] failed to read event callback: %d\n", eventCode);
        } else {
            result = callback(eventCode, data);
        }
    }

    return result;
}

u8 registerCallbackImpl(LunaEventCode eventCode, LunaEventCallback callback) {
    if (eventCode >= LUNA_EVENT_CODE_MAX || !LunaEventsInternal.eventv[eventCode]) 
        return 0;

    if (!LunaEventsInternal.callbackv[eventCode].data) 
        return 0;

    if (!r3_arr_assign(r3_arr_field(ARRAY_COUNT_FIELD, &LunaEventsInternal.callbackv[eventCode]), callback, &LunaEventsInternal.callbackv[eventCode])) {
		r3_log_stdoutf(ERROR_LOG, "[LunaEvents] failed to register event callback: (code)%d\n", eventCode);
		return 0;
	}
    return 1;
}

u8 unregisterCallbackImpl(LunaEventCode eventCode, LunaEventCallback callback) {
    if (eventCode >= LUNA_EVENT_CODE_MAX || !LunaEventsInternal.eventv[eventCode]) {
        return 0;
    }

    u16 count = r3_arr_field(ARRAY_COUNT_FIELD, &LunaEventsInternal.callbackv[eventCode]);
    if (count == 0) return 0;

    FOR_I(0, count, 1) {
        LunaEventCallback readback = {0};
        if (((LunaEventCallback*)LunaEventsInternal.callbackv[eventCode].data)[count] == callback) {
            r3_arr_pull(count, &readback, &LunaEventsInternal.callbackv[eventCode]);
            return 1;
        }
    }
    return 0;
}


u8 lunaInitEvents(LunaEvents* table) {
    if (!table) {
        r3_log_stdout(ERROR_LOG, "[LunaEvents] invalid ptr :: lunaInitEvents()\n");
        return 0;
    }

    FOR_I(0, LUNA_EVENT_CODES, 1) {
        if (!registerEventImpl(i)) {
            r3_log_stdoutf(ERROR_LOG, "[LunaEvents] failed to register: %u\n", i);
        } else r3_log_stdoutf(SUCCESS_LOG, "[LunaEvents] registered: %u\n", i);
    }

    table->pushEvent = pushEventImpl;
    table->registerEvent = registerEventImpl;
    table->unregisterEvent = unregisterEventImpl;

    table->registerCallback = registerCallbackImpl;
    table->unregisterCallback = unregisterCallbackImpl;

    // assign global dispatch table ptr
    lunaEvents = table;

    r3_log_stdout(SUCCESS_LOG, "[LunaEvents] table initialized\n");

    return 1;
}

u8 lunaDeinitEvents(LunaEvents* table) {
    FOR_I(0, LUNA_EVENT_CODE_MAX, 1) {
        if (LunaEventsInternal.eventv[i]) {
            unregisterEventImpl(i);
        }
    }
    // null global dispatch table ptr
    lunaEvents = NULL;

    table->pushEvent = NULL;
    table->registerEvent = NULL;
    table->unregisterEvent = NULL;

    table->registerCallback = NULL;
    table->unregisterCallback = NULL;

    r3_log_stdout(SUCCESS_LOG, "[LunaEvents] table deinitialized\n");

    return 1;
}
