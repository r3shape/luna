#include <include/luna/core/events.h>
#include <include/r3kt/io/log.h>
#include <include/r3kt/ds/arr.h>

static struct LunaEventsInternal {
    u16 events;
    u8 eventv[LUNA_EVENT_CODE_MAX];
    Array callbackv[LUNA_EVENT_CODE_MAX];
} LunaEventsInternal = {0};

// global dispatch table ptr
LunaEventApi* lunaEventApi = NULL;


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


u8 lunaInitEvents(none) {
    if (lunaEventApi == NULL) {
        lunaEventApi = r3_mem_alloc(sizeof(LunaEventApi), 8);
        if (lunaEventApi == NULL) {
            r3_log_stdout(ERROR_LOG, "[LunaEvents] failed to allocate lunaEventApi dispatch table!\n");
            return 0;
        } 
    }

    lunaEventApi->pushEvent = pushEventImpl;
    lunaEventApi->registerEvent = registerEventImpl;
    lunaEventApi->unregisterEvent = unregisterEventImpl;

    lunaEventApi->registerCallback = registerCallbackImpl;
    lunaEventApi->unregisterCallback = unregisterCallbackImpl;

    FOR_I(0, LUNA_EVENT_CODES, 1) {
        if (!lunaEventApi->registerEvent(i)) {
            r3_log_stdoutf(ERROR_LOG, "[LunaEvents] failed to register: %u\n", i);
        } else r3_log_stdoutf(SUCCESS_LOG, "[LunaEvents] registered: %u\n", i);
    }

    r3_log_stdout(SUCCESS_LOG, "[LunaEvents] initialized lunaEventApi\n");
    return 1;
}

u8 lunaDeinitEvents(none) {
    if (lunaEventApi != NULL) {
        FOR_I(0, LUNA_EVENT_CODE_MAX, 1) {
            if (LunaEventsInternal.eventv[i]) {
                lunaEventApi->unregisterEvent(i);
            }
        }
        r3_mem_dealloc(lunaEventApi);
        lunaEventApi = NULL;
    }

    r3_log_stdout(SUCCESS_LOG, "[LunaEvents] deinitialized lunaEventApi\n");
    return 1;
}
