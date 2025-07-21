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


// dispatch table ptrs
LunaPlatformApi* lunaPlatformApi = NULL;


LunaWindow* getWindowImpl(none) {
    return LunaPlatformInternal.window;
}

u8 getWindowFlagImpl(u32 flag) {
    if (!LunaPlatformInternal.handle) return 0;    // error: window not yet created!
    return ((LunaPlatformInternal.window->flags & flag) == flag) ? 1 : 0;
}

u8 setWindowFlagImpl(u32 flag) {
    if (!LunaPlatformInternal.handle) return 0;    // error: window not yet created!
    LunaPlatformInternal.window->flags |= flag;
    return 1;
}

u8 remWindowFlagImpl(u32 flag) {
    if (!LunaPlatformInternal.handle) return 0;    // error: window not yet created!
    LunaPlatformInternal.window->flags &= ~flag;
    return 1;
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
            lunaEventApi->pushEvent(LUNA_EVENT_EXIT, (LunaEvent){.u8[0]=1}),
            PostQuitMessage(0);
            return 0;
        }
        
        case WM_SIZE:       // fall through to WMEXITSIZEMOVE
        case WM_MOVE:       // fall through to WMEXITSIZEMOVE
        case WM_EXITSIZEMOVE: {
            RECT newRect = {0};
            GetWindowRect(handle, &newRect);
            // VEC_X(LunaPlatformInternal.window->size) = newRect.right;
            // VEC_Y(LunaPlatformInternal.window->size) = newRect.bottom;
            // VEC_Y(LunaPlatformInternal.window->location) = newRect.top;
            // VEC_X(LunaPlatformInternal.window->location) = newRect.left;
            
            if ((LunaPlatformInternal.window->flags & WINDOW_BIND_CURSOR) == WINDOW_BIND_CURSOR) ClipCursor(&newRect);
            
            u16 width = newRect.right - newRect.left;
            u16 height = newRect.bottom - newRect.top;
            lunaEventApi->pushEvent(LUNA_EVENT_RESIZE, (LunaEvent){.u16[0]=width, .u16[1]=height});
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
                VEC_X(LunaPlatformInternal.window->location) = newRect.left;
                VEC_Y(LunaPlatformInternal.window->location) = newRect.top;
                VEC_X(LunaPlatformInternal.window->size) = newRect.right;
                VEC_Y(LunaPlatformInternal.window->size) = newRect.bottom;
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
            u8 pressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
            lunaInputApi->processKeyInput(key, pressed);
        } break;

        /* RAW INPUT */
        case WM_INPUT: {
            UINT dwSize;
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
            
            RAWINPUT raw;
            if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &raw, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize) {
                r3_log_stdout(ERROR_LOG, "[Platform] GetRawInputData returned incorrect size\n");
                return DefWindowProcA(handle, msg, wParam, lParam);
            }

            if (raw.header.dwType == RIM_TYPEMOUSE) {
                RAWMOUSE *mouse = &raw.data.mouse;

                // handle mouse movement
                if (mouse->usFlags & MOUSE_MOVE_ABSOLUTE) {
                    // ignore absolute movement (used for remote desktop, etc.)
                } else {
                    s16 x = (s16)mouse->lLastX;
                    s16 y = (s16)mouse->lLastY;
                    lunaInputApi->processMouseMoveInput(x, y);
                }

                // handle mouse buttons
                if (mouse->usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) {
                    lunaInputApi->processMouseButtonInput(LUNA_MBUTTON_LEFT, 1);
                }
                if (mouse->usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) {
                    lunaInputApi->processMouseButtonInput(LUNA_MBUTTON_LEFT, 0);
                }
                if (mouse->usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) {
                    lunaInputApi->processMouseButtonInput(LUNA_MBUTTON_RIGHT, 1);
                }
                if (mouse->usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) {
                    lunaInputApi->processMouseButtonInput(LUNA_MBUTTON_RIGHT, 0);
                }
                if (mouse->usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) {
                    lunaInputApi->processMouseButtonInput(LUNA_MBUTTON_MIDDLE, 1);
                }
                if (mouse->usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP) {
                    lunaInputApi->processMouseButtonInput(LUNA_MBUTTON_MIDDLE, 0);
                }

                // handle mouse wheel
                if (mouse->usButtonFlags & RI_MOUSE_WHEEL) {
                    s32 z = (s32)((SHORT)mouse->usButtonData) / WHEEL_DELTA;
                    lunaInputApi->processMouseWheelInput(z);
                }
            }
        } break;
    }
    return DefWindowProcA(handle, msg, wParam, lParam);
}

u8 createWindowImpl(str title, u32 width, u32 height, u32 x, u32 y, LunaWindow* window) {
    if (!window) {
        r3_log_stdout(ERROR_LOG, "[LunaPlatform] invalid ptr :: createWindowImpl()\n");
        return 0;
    }
    
    // Set internal window pointer
    LunaPlatformInternal.window = window;

    if (LunaPlatformInternal.handle != NULL) {
        r3_log_stdout(WARN_LOG, "[LunaPlatform] window already created\n");
        return 1;  // error: window already created!
    }

    // Register window class
    WNDCLASS wc = {0};
    wc.lpfnWndProc = windowProc;
    wc.lpszClassName = "LunaWindow";
    wc.hInstance = LunaPlatformInternal.instance;

    if (!RegisterClass(&wc)) {
        r3_log_stdoutf(ERROR_LOG, "[LunaPlatform] failed to register window class (err=%lu)\n", GetLastError());
        return 0;
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
        r3_log_stdout(ERROR_LOG, "[LunaPlatform] failed to create window class\n");
        return 0; // error: failed to create window class!
    }

    LunaPlatformInternal.glContext = NULL;
    LunaPlatformInternal.deviceContext = GetDC(LunaPlatformInternal.handle);

    // Register rawinput devices
    LunaPlatformInternal.rid.usUsagePage = 0x01;       // HID_USAGE_PAGE_GENERIC
    LunaPlatformInternal.rid.usUsage = 0x02;           // HID_USAGE_GENERIC_MOUSE
    LunaPlatformInternal.rid.dwFlags = 0;              // RIDEV_NOLEGACY adds raw mouse and ignores legacy mouse messages
    LunaPlatformInternal.rid.hwndTarget = LunaPlatformInternal.handle;

    if (!RegisterRawInputDevices(&LunaPlatformInternal.rid, 1, sizeof(LunaPlatformInternal.rid))) {
        r3_log_stdout(ERROR_LOG, "[LunaPlatform] failed to register rawinput devices\n");
        return 0;    // error: failed to register rawinput devices!
    }

    // Initialize the window structure
    LunaPlatformInternal.window->flags = 0;
    LunaPlatformInternal.window->title = title;
    VEC_X(LunaPlatformInternal.window->location) = x;
    VEC_Y(LunaPlatformInternal.window->location) = y;
    VEC_X(LunaPlatformInternal.window->size) = width;
    VEC_Y(LunaPlatformInternal.window->size) = height;
    LunaPlatformInternal.window->aspect = width / height;
    
    // Set default window flags
    setWindowFlagImpl(WINDOW_FOCUSED);
    setWindowFlagImpl(WINDOW_SHOW_CURSOR);

    return 1;
}

u8 createGLContextImpl(void) {
    if (!LunaPlatformInternal.handle) return 0;    // error: window not yet created!

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
        return 0;  // error: failed to set pixel format!
    }

    LunaPlatformInternal.glContext = wglCreateContext(LunaPlatformInternal.deviceContext);
    if (!LunaPlatformInternal.glContext) {
        return 0;  // error: failed to create GL context!
    }

    if (!wglMakeCurrent(LunaPlatformInternal.deviceContext, LunaPlatformInternal.glContext)) {
        wglDeleteContext(LunaPlatformInternal.glContext);
        return 0;  // error: failed to make context current!
    }

    return 1;
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
    lunaInputApi->update();
}


u8 loadLibraryImpl(str path, str name, LunaLibrary* library) {
    char full_path[MAX_PATH];
    if (path != NULL) {
        // TODO: levarage r3kt strings instead of snprintf!!!
        snprintf(full_path, sizeof(full_path), "%s/%s.dll", path, name);
    } else snprintf(full_path, sizeof(full_path), "%s.dll", name);
    
    library->handle = LoadLibrary(full_path);
    if (!library->handle) {
        r3_log_stdoutf(ERROR_LOG, "[LunaPlatform] failed to load library: %s\n", name);
        return 0; // error: failed to load library!
    }

    library->name = name;

    return 1;
}

u8 loadLibrarySymbolImpl(str name, ptr* symbol, LunaLibrary* library) {
    if (!library || !library->handle || !name) {
        r3_log_stdoutf(ERROR_LOG, "[LunaPlatform] failed to load symbol: %s\n", name);
        return 0; // error: null ptr!
    }

    *symbol = wglGetProcAddress(name);
    if ((*symbol == NULL)     ||
        (*symbol == (ptr)0x1) ||
        (*symbol == (ptr)0x2) ||
        (*symbol == (ptr)0x3) ||
        (*symbol == (ptr)-1)) {
        *symbol = GetProcAddress((HMODULE)library->handle, name);
    }

    if (*symbol == NULL) {
        r3_log_stdoutf(ERROR_LOG, "[LunaPlatform] failed to load symbol: %s\n", name);
        return 0;
    } else {
        r3_log_stdoutf(INFO_LOG, "[LunaPlatform] loaded symbol: (name)%s (ptr)%p\n", name, *symbol);
        return 1;
    }
}

u8 unloadLibraryImpl(LunaLibrary* library) {
    if (!library || !library->handle) {
        r3_log_stdout(ERROR_LOG, "[LunaPlatform] invalid ptr :: unloadLibraryImpl()\n");
        return 0;    // error: null ptr!
    }
    if (!FreeLibrary((HMODULE)library->handle)) {
        r3_log_stdout(ERROR_LOG, "[LunaPlatform] failed to free library\n");
        return 0;  // error: failed to free library!
    }

    library->name = NULL;
    library->handle = NULL;
    
    return 1;
}


u8 lunaInitPlatform(none) {
    if (lunaEventApi == NULL) {
        r3_log_stdout(ERROR_LOG, "[LunaPlatform] LunaEventApi not initialized!\n");
        return 0;
    }
    
    if (lunaInputApi == NULL) {
        r3_log_stdout(ERROR_LOG, "[LunaPlatform] LunaInputApi not initialized!\n");
        return 0;
    }

    if (lunaPlatformApi == NULL) {
        lunaPlatformApi = r3_mem_alloc(sizeof(LunaPlatformApi), 8);
        if (lunaPlatformApi == NULL) {
            r3_log_stdout(ERROR_LOG, "[LunaPlatform] failed to allocate lunaPlatformApi dispatch table!\n");
            return 0;
        }
    }

    lunaPlatformApi->createWindow = createWindowImpl;
    lunaPlatformApi->destroyWindow = destroyWindowImpl;
    
    lunaPlatformApi->swapBuffers = swapBuffersImpl;
    lunaPlatformApi->createGLContext = createGLContextImpl;
    lunaPlatformApi->destroyGLContext = destroyGLContextImpl;
    
    lunaPlatformApi->pollEvents = pollEventsImpl;
    lunaPlatformApi->pollInputs = pollInputsImpl;
    
    lunaPlatformApi->getWindow = getWindowImpl;
    lunaPlatformApi->getWindowFlag = getWindowFlagImpl;
    lunaPlatformApi->setWindowFlag = setWindowFlagImpl;
    lunaPlatformApi->remWindowFlag = remWindowFlagImpl;
    
    lunaPlatformApi->loadLibrary = loadLibraryImpl;
    lunaPlatformApi->unloadLibrary = unloadLibraryImpl;
    lunaPlatformApi->loadLibrarySymbol = loadLibrarySymbolImpl;

    r3_log_stdout(SUCCESS_LOG, "[LunaPlatform] initialized lunaPlatformApi\n");
    return 1;
}

u8 lunaDeinitPlatform(none) {
    if (lunaPlatformApi != NULL) {
        r3_mem_dealloc(lunaPlatformApi);
        lunaPlatformApi = NULL;
    }

    r3_log_stdout(SUCCESS_LOG, "[LunaPlatform] deinitialized lunaPlatformApi\n");
    return 1;
}

#endif  // LUNA_PLATFORM_WINDOWS