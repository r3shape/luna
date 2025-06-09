#include <include/luna/core/events.h>

static struct LunaEventsInternal {
    u16 events;
    u8 eventv[LUNA_EVENT_CODE_MAX];
    LunaEventCallback* callbackv[LUNA_EVENT_CODE_MAX];
} LunaEventsInternal = {0};


u8 registerEventImpl(LunaEventCode eventCode) {
    if (eventCode >= LUNA_EVENT_CODE_MAX || LunaEventsInternal.eventv[eventCode]) 
        return SSDK_FALSE;

    LunaEventsInternal.eventv[eventCode] = SSDK_TRUE;
    LunaEventsInternal.callbackv[eventCode] = saneData->array.create(sizeof(LunaEventCallback), LUNA_EVENT_CALLBACK_MAX);

    LunaEventsInternal.events++;
    return SSDK_TRUE;
}

u8 unregisterEventImpl(LunaEventCode eventCode) {
    if (eventCode >= LUNA_EVENT_CODE_MAX || !LunaEventsInternal.eventv[eventCode]) 
        return SSDK_FALSE;

    LunaEventsInternal.eventv[eventCode] = SSDK_FALSE;
    
    if (LunaEventsInternal.callbackv[eventCode]) {
        saneData->array.destroy(LunaEventsInternal.callbackv[eventCode]);
        LunaEventsInternal.callbackv[eventCode] = NULL;
    }

    LunaEventsInternal.events--;
    return SSDK_TRUE;
}

u8 pushEventImpl(LunaEventCode eventCode, LunaEvent data) {
    if (eventCode >= LUNA_EVENT_CODE_MAX || !LunaEventsInternal.eventv[eventCode]) 
        return SSDK_FALSE;

    if (!LunaEventsInternal.callbackv[eventCode]) return SSDK_FALSE;

    u8 result = SSDK_FALSE;
    ArrayHeader head = saneData->array.getHeader(LunaEventsInternal.callbackv[eventCode]);
    SSDK_FORI(0, head.count, 1) {
        result = LunaEventsInternal.callbackv[eventCode][i](eventCode, data);
    }

    return result;
}

u8 registerCallbackImpl(LunaEventCode eventCode, LunaEventCallback callback) {
    if (eventCode >= LUNA_EVENT_CODE_MAX || !LunaEventsInternal.eventv[eventCode]) 
        return SSDK_FALSE;

    if (!LunaEventsInternal.callbackv[eventCode]) 
        return SSDK_FALSE;

    saneData->array.push(LunaEventsInternal.callbackv[eventCode], &callback);
    return SSDK_TRUE;
}

u8 unregisterCallbackImpl(LunaEventCode eventCode, LunaEventCallback callback) {
    if (eventCode >= LUNA_EVENT_CODE_MAX || !LunaEventsInternal.eventv[eventCode]) 
        return SSDK_FALSE;

    ArrayHeader head = saneData->array.getHeader(LunaEventsInternal.callbackv[eventCode]);
    if (head.count == 0) return SSDK_FALSE;

    SSDK_FORI(0, head.count, 1) {
        if (LunaEventsInternal.callbackv[eventCode][i] == callback) {
            LunaEventCallback callback;
            saneData->array.pull(LunaEventsInternal.callbackv[eventCode], i, &callback);
            return SSDK_TRUE;
        }
    }
    return SSDK_FALSE;
}


byte lunaInitEvents(LunaEvents* table) {
    if (!table) {
        saneLog->log(SANE_LOG_ERROR, "[LunaEvents] invalid ptr :: lunaInitEvents()");
        return SSDK_FALSE;
    }

    SSDK_FORI(0, LUNA_EVENT_CODES, 1) {
        if (!registerEventImpl(i)) {
            saneLog->logFmt(SANE_LOG_ERROR, "[LunaEvents] failed to register: %u", i);
        } else saneLog->logFmt(SANE_LOG_SUCCESS, "[LunaEvents] registered: %u", i);
    }

    table->pushEvent = pushEventImpl;
    table->registerEvent = registerEventImpl;
    table->unregisterEvent = unregisterEventImpl;

    table->registerCallback = registerCallbackImpl;
    table->unregisterCallback = unregisterCallbackImpl;

    saneLog->log(SANE_LOG_SUCCESS, "[LunaEvents] table initialized");

    return SSDK_TRUE;
}

byte lunaDeinitEvents(LunaEvents* table) {
    SSDK_FORI(0, LUNA_EVENT_CODE_MAX, 1) {
        if (LunaEventsInternal.eventv[i]) {
            unregisterEventImpl(i);
        }
    }

    table->pushEvent = NULL;
    table->registerEvent = NULL;
    table->unregisterEvent = NULL;
    
    table->registerCallback = NULL;
    table->unregisterCallback = NULL;

    saneLog->log(SANE_LOG_SUCCESS, "[LunaEvents] table deinitialized");

    return SSDK_TRUE;
}
