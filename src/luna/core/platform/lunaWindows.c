#include <include/luna/core/platform.h>
#include <include/luna/core/events.h>
#include <include/luna/core/inputs.h>

#if defined(LUNA_PLATFORM_WINDOWS)
#include <stdio.h>
#include <Windows.h>
#include <windowsx.h>   // parameter input extraction

static struct LunaPlatformInternal {
    HWND handle;
    HGLRC glContext;
    HDC deviceContext;
    HINSTANCE instance;
    RAWINPUTDEVICE rid;
    LunaWindow* window;
} LunaPlatformInternal = {0};

// internal dispatch table ptrs
static LunaEvents* platformEvents = NULL;
static LunaInputs* platformInputs = NULL;


byte getWindowFlagImpl(u32 flag) {
    if (!LunaPlatformInternal.handle) return SSDK_FALSE;    // error: window not yet created!
    return ((LunaPlatformInternal.window->flags & flag) == flag) ? SSDK_TRUE : SSDK_FALSE;
}

byte setWindowFlagImpl(u32 flag) {
    if (!LunaPlatformInternal.handle) return SSDK_FALSE;    // error: window not yet created!
    LunaPlatformInternal.window->flags |= flag;
    return SSDK_TRUE;
}

byte remWindowFlagImpl(u32 flag) {
    if (!LunaPlatformInternal.handle) return SSDK_FALSE;    // error: window not yet created!
    LunaPlatformInternal.window->flags &= ~flag;
    return SSDK_TRUE;
}


LRESULT CALLBACK windowProc(HWND handle, u32 msg, WPARAM wParam, LPARAM lParam) {
    if (!handle) return DefWindowProcA(handle, msg, wParam, lParam);    // error: how did you get here?

    RECT window_rect;
    GetWindowRect(LunaPlatformInternal.handle, &window_rect);

    // only handle window flags during focus
    if (getWindowFlagImpl(WINDOW_FOCUSED)) {
        ShowCursor(getWindowFlagImpl(WINDOW_SHOW_CURSOR));
        
        if (getWindowFlagImpl(WINDOW_BIND_CURSOR)) {
            ClipCursor(&window_rect);
        }
        
        if (getWindowFlagImpl(WINDOW_CENTER_CURSOR)) {
            SetCursorPos((window_rect.left + window_rect.right) / 2, (window_rect.top + window_rect.bottom) / 2);
        }
    }

    switch(msg) {
        case WM_ERASEBKGND: { return 1; }

        case WM_QUIT:       // fall through WM_DESTROY
        case WM_CLOSE:      // fall through WM_DESTROY
        case WM_DESTROY: {
            platformEvents->pushEvent(LUNA_EVENT_EXIT, (LunaEvent){.u8[0]=1}),
            PostQuitMessage(0);
            return 0;
        }
        
        case WM_SIZE:       // fall through to WMEXITSIZEMOVE
        case WM_MOVE:       // fall through to WMEXITSIZEMOVE
        case WM_EXITSIZEMOVE: {
            RECT newRect = {0};
            GetWindowRect(handle, &newRect);
            LunaPlatformInternal.window->size.x = newRect.right;
            LunaPlatformInternal.window->size.y = newRect.bottom;
            LunaPlatformInternal.window->location.y = newRect.top;
            LunaPlatformInternal.window->location.x = newRect.left;
            
            if ((LunaPlatformInternal.window->flags & WINDOW_BIND_CURSOR) == WINDOW_BIND_CURSOR) ClipCursor(&newRect);
            
            u16 width = newRect.right - newRect.left;
            u16 height = newRect.bottom - newRect.top;
            platformEvents->pushEvent(LUNA_EVENT_RESIZE, (LunaEvent){.u16[0]=width, .u16[1]=height});
        } break;
        
        case WM_KILLFOCUS: {
            remWindowFlagImpl(WINDOW_FOCUSED);
            ClipCursor(NULL);
        } break;
        
        case WM_SETFOCUS: {
            setWindowFlagImpl(WINDOW_FOCUSED);
            if ((LunaPlatformInternal.window->flags & WINDOW_BIND_CURSOR) == WINDOW_BIND_CURSOR) {
                RECT newRect = {0};
                GetWindowRect(handle, &newRect);
                LunaPlatformInternal.window->location.x = newRect.left;
                LunaPlatformInternal.window->location.y = newRect.top;
                LunaPlatformInternal.window->size.x = newRect.right;
                LunaPlatformInternal.window->size.y = newRect.bottom;
                ClipCursor(&newRect);
            }
        } break;
        
        /* LEGACY INPUT */
        case WM_KEYUP:          // fall through WM_SYSKEYUP
        case WM_KEYDOWN:        // fall through WM_SYSKEYUP
        case WM_SYSKEYDOWN:     // fall through WM_SYSKEYUP
        case WM_SYSKEYUP: {
            // key pressed/released
            LunaKeyboardKey key = (u16)wParam;
            byte pressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
            platformInputs->processKeyInput(key, pressed);
        } break;

        /* RAW INPUT */
        case WM_INPUT: {
            UINT dwSize;
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
            
            RAWINPUT raw;
            if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &raw, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize) {
                saneLog->log(SANE_LOG_ERROR, "[Platform] GetRawInputData returned incorrect size");
                return DefWindowProcA(handle, msg, wParam, lParam);
            }

            if (raw.header.dwType == RIM_TYPEMOUSE) {
                RAWMOUSE *mouse = &raw.data.mouse;

                // handle mouse movement
                if (mouse->usFlags & MOUSE_MOVE_ABSOLUTE) {
                    // ignore absolute movement (used for remote desktop, etc.)
                } else {
                    i16 x = (i16)mouse->lLastX;
                    i16 y = (i16)mouse->lLastY;
                    platformInputs->processMouseMoveInput(x, y);
                }

                // handle mouse buttons
                if (mouse->usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) {
                    platformInputs->processMouseButtonInput(LUNA_MBUTTON_LEFT, 1);
                }
                if (mouse->usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) {
                    platformInputs->processMouseButtonInput(LUNA_MBUTTON_LEFT, 0);
                }
                if (mouse->usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) {
                    platformInputs->processMouseButtonInput(LUNA_MBUTTON_RIGHT, 1);
                }
                if (mouse->usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) {
                    platformInputs->processMouseButtonInput(LUNA_MBUTTON_RIGHT, 0);
                }
                if (mouse->usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) {
                    platformInputs->processMouseButtonInput(LUNA_MBUTTON_MIDDLE, 1);
                }
                if (mouse->usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP) {
                    platformInputs->processMouseButtonInput(LUNA_MBUTTON_MIDDLE, 0);
                }

                // handle mouse wheel
                if (mouse->usButtonFlags & RI_MOUSE_WHEEL) {
                    i32 z = (i32)((SHORT)mouse->usButtonData) / WHEEL_DELTA;
                    platformInputs->processMouseWheelInput(z);
                }
            }
        } break;
    }
    return DefWindowProcA(handle, msg, wParam, lParam);
}

byte createWindowImpl(str title, u32 width, u32 height, u32 x, u32 y, LunaWindow* window) {
    if (!window) {
        saneLog->log(SANE_LOG_ERROR, "[LunaPlatform] invalid ptr :: createWindowImpl()");
        return SSDK_FALSE;
    }
    
    // Set internal window pointer
    LunaPlatformInternal.window = window;

    if (LunaPlatformInternal.handle != NULL) {
        saneLog->log(SANE_LOG_WARN, "[LunaPlatform] window already created");
        return SSDK_TRUE;  // error: window already created!
    }

    // Register window class
    WNDCLASS wc = {0};
    wc.lpfnWndProc = windowProc;
    wc.lpszClassName = "LunaWindow";
    wc.hInstance = LunaPlatformInternal.instance;

    if (!RegisterClass(&wc)) {
        saneLog->logFmt(SANE_LOG_ERROR, "[LunaPlatform] failed to register window class (err=%lu)", GetLastError());
        return SSDK_FALSE;
    }

    // Create the window
    LunaPlatformInternal.handle = CreateWindow(
        "LunaWindow", title,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        x, y,
        width, height,
        NULL,   // parent
        NULL,   // menu
        LunaPlatformInternal.instance,
        NULL
    );
    
    if (!LunaPlatformInternal.handle) {
        saneLog->log(SANE_LOG_ERROR, "[LunaPlatform] failed to create window class");
        return SSDK_FALSE; // error: failed to create window class!
    }

    LunaPlatformInternal.glContext = NULL;
    LunaPlatformInternal.deviceContext = GetDC(LunaPlatformInternal.handle);

    // Register rawinput devices
    LunaPlatformInternal.rid.usUsagePage = 0x01;       // HID_USAGE_PAGE_GENERIC
    LunaPlatformInternal.rid.usUsage = 0x02;           // HID_USAGE_GENERIC_MOUSE
    LunaPlatformInternal.rid.dwFlags = 0;              // RIDEV_NOLEGACY adds raw mouse and ignores legacy mouse messages
    LunaPlatformInternal.rid.hwndTarget = LunaPlatformInternal.handle;

    if (!RegisterRawInputDevices(&LunaPlatformInternal.rid, 1, sizeof(LunaPlatformInternal.rid))) {
        saneLog->log(SANE_LOG_ERROR, "[LunaPlatform] failed to register rawinput devices");
        return SSDK_FALSE;    // error: failed to register rawinput devices!
    }

    // Initialize the window structure
    LunaPlatformInternal.window->flags = 0;
    LunaPlatformInternal.window->title = title;
    LunaPlatformInternal.window->location.x = x;
    LunaPlatformInternal.window->location.y = y;
    LunaPlatformInternal.window->size.x = width;
    LunaPlatformInternal.window->size.y = height;
    LunaPlatformInternal.window->aspect = width / height;
    
    // Set default window flags
    setWindowFlagImpl(WINDOW_FOCUSED);
    setWindowFlagImpl(WINDOW_SHOW_CURSOR);

    return SSDK_TRUE;
}

byte createGLContextImpl(void) {
    if (!LunaPlatformInternal.handle) return SSDK_FALSE;    // error: window not yet created!

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32,
        0, 0, 0, 0, 0, 0,
        0, 0,
        0, 0, 0, 0, 0,
        24, 8,
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

    int format = ChoosePixelFormat(LunaPlatformInternal.deviceContext, &pfd);
    if (!SetPixelFormat(LunaPlatformInternal.deviceContext, format, &pfd)) {
        return SSDK_FALSE;  // error: failed to set pixel format!
    }

    LunaPlatformInternal.glContext = wglCreateContext(LunaPlatformInternal.deviceContext);
    if (!LunaPlatformInternal.glContext) {
        return SSDK_FALSE;  // error: failed to create GL context!
    }

    if (!wglMakeCurrent(LunaPlatformInternal.deviceContext, LunaPlatformInternal.glContext)) {
        wglDeleteContext(LunaPlatformInternal.glContext);
        return SSDK_FALSE;  // error: failed to make context current!
    }

    return SSDK_TRUE;
}

void swapBuffersImpl(void) {
    if (!LunaPlatformInternal.handle) return;    // error: no window created!
    SwapBuffers(LunaPlatformInternal.deviceContext);
}

void destroyGLContextImpl(void) {
    if (!LunaPlatformInternal.handle) return;    // error: no window created!

    if (LunaPlatformInternal.glContext) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(LunaPlatformInternal.glContext);
        LunaPlatformInternal.glContext = NULL;
    }
}

void destroyWindowImpl(void) {
    if (!LunaPlatformInternal.handle) return;    // error: window not yet created!
    DestroyWindow(LunaPlatformInternal.handle);
    if (LunaPlatformInternal.glContext) destroyGLContextImpl();
}


void pollEventsImpl(void) {
    MSG message;
    while(PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
}

void pollInputsImpl(void) {
    platformInputs->update();
}


byte loadLibraryImpl(str path, str name, LunaLibrary* library) {
    char full_path[MAX_PATH];
    if (path != NULL) {
        // TODO: levarage SSDK strings instead of snprintf!!!
        snprintf(full_path, sizeof(full_path), "%s/%s.dll", path, name);
    } else snprintf(full_path, sizeof(full_path), "%s.dll", name);
    
    library->handle = LoadLibrary(full_path);
    if (!library->handle) {
        saneLog->logFmt(SANE_LOG_ERROR, "[LunaPlatform] failed to load library: %s", name);
        return SSDK_FALSE; // error: failed to load library!
    }

    library->name = name;

    return SSDK_TRUE;
}

byte loadLibrarySymbolImpl(str name, ptr* symbol, LunaLibrary* library) {
    if (!library || !library->handle || !name) {
        saneLog->logFmt(SANE_LOG_ERROR, "[LunaPlatform] failed to load symbol: %s", name);
        return SSDK_FALSE; // error: null ptr!
    }

    *symbol = wglGetProcAddress(name);
    if (*symbol == NULL       ||
        (*symbol == (ptr)0x1) ||
        (*symbol == (ptr)0x2) ||
        (*symbol == (ptr)0x3) ||
        (*symbol == (ptr)-1)) {
        *symbol = GetProcAddress((HMODULE)library->handle, name);
    }

    if (*symbol == NULL) {
        saneLog->logFmt(SANE_LOG_ERROR, "[LunaPlatform] failed to load symbol: %s", name);
        return SSDK_FALSE; // error: null ptr!
    } else return SSDK_TRUE;
}

byte unloadLibraryImpl(LunaLibrary* library) {
    if (!library || !library->handle) {
        saneLog->log(SANE_LOG_ERROR, "[LunaPlatform] invalid ptr :: unloadLibraryImpl()");
        return SSDK_FALSE;    // error: null ptr!
    }
    if (!FreeLibrary((HMODULE)library->handle)) {
        saneLog->log(SANE_LOG_ERROR, "[LunaPlatform] failed to free library");
        return SSDK_FALSE;  // error: failed to free library!
    }

    library->name = NULL;
    library->handle = NULL;
    
    return SSDK_TRUE;
}


byte lunaInitPlatform(LunaPlatform* table, ptr events_table, ptr inputs_table) {
    if (!table || !events_table || !inputs_table) {
        saneLog->log(SANE_LOG_ERROR, "[LunaPlatform] invalid ptr :: lunaInitPlatform()");
        return SSDK_FALSE;
    }

    // assign internal dispatch table ptrs
    platformEvents = (LunaEvents*)events_table;
    platformInputs = (LunaInputs*)inputs_table;

    table->createWindow = createWindowImpl;
    table->destroyWindow = destroyWindowImpl;
    
    table->swapBuffers = swapBuffersImpl;
    table->createGLContext = createGLContextImpl;
    table->destroyGLContext = destroyGLContextImpl;
    
    table->pollEvents = pollEventsImpl;
    table->pollInputs = pollInputsImpl;
    
    table->getWindowFlag = getWindowFlagImpl;
    table->setWindowFlag = setWindowFlagImpl;
    table->remWindowFlag = remWindowFlagImpl;
    
    table->loadLibrary = loadLibraryImpl;
    table->unloadLibrary = unloadLibraryImpl;
    table->loadLibrarySymbol = loadLibrarySymbolImpl;

    saneLog->log(SANE_LOG_SUCCESS, "[LunaPlatform] table initialized");

    return SSDK_TRUE;
}

byte lunaDeinitPlatform(LunaPlatform* table) {
    if (!table) {
        saneLog->log(SANE_LOG_ERROR, "[LunaPlatform] invalid ptr :: lunaDeinitPlatform()");
        return SSDK_FALSE;
    }

    table->createWindow = NULL;
    table->destroyWindow = NULL;
    
    table->swapBuffers = NULL;
    table->createGLContext = NULL;
    table->destroyGLContext = NULL;
    
    table->pollEvents = NULL;
    table->pollInputs = NULL;
    
    table->getWindowFlag = NULL;
    table->setWindowFlag = NULL;
    table->remWindowFlag = NULL;
    
    table->loadLibrary = NULL;
    table->unloadLibrary = NULL;
    table->loadLibrarySymbol = NULL;

    saneLog->log(SANE_LOG_SUCCESS, "[LunaPlatform] table deinitialized");

    return SSDK_TRUE;
}

#endif  // LUNA_PLATFORM_WINDOWS
