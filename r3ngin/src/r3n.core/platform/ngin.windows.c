#include <r3ngin/include/r3n.core/platform.h>
#include <r3ngin/include/r3n.core/events.h>
#include <r3ngin/include/r3n.core/inputs.h>

#if defined(R3N_PLATFORM_WINDOWS)
#include <stdio.h>
#include <Windows.h>
#include <windowsx.h>   // parameter input extraction

static struct R3N_Windows_Internal {
    HWND handle;
    HGLRC glContext;
    HDC deviceContext;
    HINSTANCE instance;
    RAWINPUTDEVICE rid;
    R3N_Window* window;
} R3N_Windows_Internal = {0};


// dispatch table ptrs
R3N_Platform* R3N_Platform_Ptr = NULL;


R3N_Window* get_window_impl(none) {
    return R3N_Windows_Internal.window;
}

u8 get_window_flag_impl(u32 flag) {
    if (!R3N_Windows_Internal.handle) return 0;    // error: window not yet created!
    return ((R3N_Windows_Internal.window->flags & flag) == flag) ? 1 : 0;
}

u8 set_window_flag_impl(u32 flag) {
    if (!R3N_Windows_Internal.handle) return 0;    // error: window not yet created!
    R3N_Windows_Internal.window->flags |= flag;
    return 1;
}

u8 rem_window_flag_impl(u32 flag) {
    if (!R3N_Windows_Internal.handle) return 0;    // error: window not yet created!
    R3N_Windows_Internal.window->flags &= ~flag;
    return 1;
}


LRESULT CALLBACK windowProc(HWND handle, u32 msg, WPARAM wParam, LPARAM lParam) {
    if (!handle) return DefWindowProcA(handle, msg, wParam, lParam);    // error: how did you get here?

    RECT window_rect;
    GetWindowRect(R3N_Windows_Internal.handle, &window_rect);

    // only handle window flags during focus
    if (get_window_flag_impl(WINDOW_FOCUSED)) {
        ShowCursor(get_window_flag_impl(WINDOW_SHOW_CURSOR));
        
        if (get_window_flag_impl(WINDOW_BIND_CURSOR)) {
            ClipCursor(&window_rect);
        }
        
        if (get_window_flag_impl(WINDOW_CENTER_CURSOR)) {
            SetCursorPos((window_rect.left + window_rect.right) / 2, (window_rect.top + window_rect.bottom) / 2);
        }
    }

    switch(msg) {
        case WM_ERASEBKGND: { return 1; }

        case WM_QUIT:       // fall through WM_DESTROY
        case WM_CLOSE:      // fall through WM_DESTROY
        case WM_DESTROY: {
            R3N_Events_Ptr->push_event(R3N_EVENT_EXIT, (R3N_Event){.u8[0]=1}),
            PostQuitMessage(0);
            return 0;
        }
        
        case WM_SIZE:       // fall through to WMEXITSIZEMOVE
        case WM_MOVE:       // fall through to WMEXITSIZEMOVE
        case WM_EXITSIZEMOVE: {
            RECT newRect = {0};
            GetWindowRect(handle, &newRect);
            // VEC_X(R3N_Windows_Internal.window->size) = newRect.right;
            // VEC_Y(R3N_Windows_Internal.window->size) = newRect.bottom;
            // VEC_Y(R3N_Windows_Internal.window->location) = newRect.top;
            // VEC_X(R3N_Windows_Internal.window->location) = newRect.left;
            
            if ((R3N_Windows_Internal.window->flags & WINDOW_BIND_CURSOR) == WINDOW_BIND_CURSOR) ClipCursor(&newRect);
            
            u16 width = newRect.right - newRect.left;
            u16 height = newRect.bottom - newRect.top;
            R3N_Events_Ptr->push_event(R3N_EVENT_RESIZE, (R3N_Event){.u16[0]=width, .u16[1]=height});
        } break;
        
        case WM_KILLFOCUS: {
            rem_window_flag_impl(WINDOW_FOCUSED);
            ClipCursor(NULL);
        } break;
        
        case WM_SETFOCUS: {
            set_window_flag_impl(WINDOW_FOCUSED);
            if ((R3N_Windows_Internal.window->flags & WINDOW_BIND_CURSOR) == WINDOW_BIND_CURSOR) {
                RECT newRect = {0};
                GetWindowRect(handle, &newRect);
                VEC_X(R3N_Windows_Internal.window->location) = newRect.left;
                VEC_Y(R3N_Windows_Internal.window->location) = newRect.top;
                VEC_X(R3N_Windows_Internal.window->size) = newRect.right;
                VEC_Y(R3N_Windows_Internal.window->size) = newRect.bottom;
                ClipCursor(&newRect);
            }
        } break;
        
        /* LEGACY INPUT */
        case WM_KEYUP:          // fall through WM_SYSKEYUP
        case WM_KEYDOWN:        // fall through WM_SYSKEYUP
        case WM_SYSKEYDOWN:     // fall through WM_SYSKEYUP
        case WM_SYSKEYUP: {
            // key pressed/released
            R3N_Keyboard_Key key = (u16)wParam;
            u8 pressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
            R3N_Inputs_Ptr->process_key_input(key, pressed);
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
                    R3N_Inputs_Ptr->process_mouse_move_input(x, y);
                }

                // handle mouse buttons
                if (mouse->usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) {
                    R3N_Inputs_Ptr->process_mouse_button_input(R3N_MBUTTON_LEFT, 1);
                }
                if (mouse->usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) {
                    R3N_Inputs_Ptr->process_mouse_button_input(R3N_MBUTTON_LEFT, 0);
                }
                if (mouse->usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) {
                    R3N_Inputs_Ptr->process_mouse_button_input(R3N_MBUTTON_RIGHT, 1);
                }
                if (mouse->usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) {
                    R3N_Inputs_Ptr->process_mouse_button_input(R3N_MBUTTON_RIGHT, 0);
                }
                if (mouse->usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) {
                    R3N_Inputs_Ptr->process_mouse_button_input(R3N_MBUTTON_MIDDLE, 1);
                }
                if (mouse->usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP) {
                    R3N_Inputs_Ptr->process_mouse_button_input(R3N_MBUTTON_MIDDLE, 0);
                }

                // handle mouse wheel
                if (mouse->usButtonFlags & RI_MOUSE_WHEEL) {
                    s32 z = (s32)((SHORT)mouse->usButtonData) / WHEEL_DELTA;
                    R3N_Inputs_Ptr->process_mouse_wheel_input(z);
                }
            }
        } break;
    }
    return DefWindowProcA(handle, msg, wParam, lParam);
}

u8 create_window_impl(str title, u32 width, u32 height, u32 x, u32 y, R3N_Window* window) {
    if (!window) {
        r3_log_stdout(ERROR_LOG, "[R3N_Platform] invalid ptr :: create_window_impl()\n");
        return 0;
    }
    
    // Set internal window pointer
    R3N_Windows_Internal.window = window;

    if (R3N_Windows_Internal.handle != NULL) {
        r3_log_stdout(WARN_LOG, "[R3N_Platform] window already created\n");
        return 1;  // error: window already created!
    }

    // Register window class
    WNDCLASS wc = {0};
    wc.lpfnWndProc = windowProc;
    wc.lpszClassName = "R3N_Window";
    wc.hInstance = R3N_Windows_Internal.instance;

    if (!RegisterClass(&wc)) {
        r3_log_stdoutf(ERROR_LOG, "[R3N_Platform] failed to register window class (err=%lu)\n", GetLastError());
        return 0;
    }

    // Create the window
    R3N_Windows_Internal.handle = CreateWindow(
        "R3N_Window", title,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        x, y,
        width, height,
        NULL,   // parent
        NULL,   // menu
        R3N_Windows_Internal.instance,
        NULL
    );
    
    if (!R3N_Windows_Internal.handle) {
        r3_log_stdout(ERROR_LOG, "[R3N_Platform] failed to create window class\n");
        return 0; // error: failed to create window class!
    }

    R3N_Windows_Internal.glContext = NULL;
    R3N_Windows_Internal.deviceContext = GetDC(R3N_Windows_Internal.handle);

    // Register rawinput devices
    R3N_Windows_Internal.rid.usUsagePage = 0x01;       // HID_USAGE_PAGE_GENERIC
    R3N_Windows_Internal.rid.usUsage = 0x02;           // HID_USAGE_GENERIC_MOUSE
    R3N_Windows_Internal.rid.dwFlags = 0;              // RIDEV_NOLEGACY adds raw mouse and ignores legacy mouse messages
    R3N_Windows_Internal.rid.hwndTarget = R3N_Windows_Internal.handle;

    if (!RegisterRawInputDevices(&R3N_Windows_Internal.rid, 1, sizeof(R3N_Windows_Internal.rid))) {
        r3_log_stdout(ERROR_LOG, "[R3N_Platform] failed to register rawinput devices\n");
        return 0;    // error: failed to register rawinput devices!
    }

    // Initialize the window structure
    R3N_Windows_Internal.window->flags = 0;
    R3N_Windows_Internal.window->title = title;
    VEC_X(R3N_Windows_Internal.window->location) = x;
    VEC_Y(R3N_Windows_Internal.window->location) = y;
    VEC_X(R3N_Windows_Internal.window->size) = width;
    VEC_Y(R3N_Windows_Internal.window->size) = height;
    R3N_Windows_Internal.window->aspect = width / height;
    
    // Set default window flags
    set_window_flag_impl(WINDOW_FOCUSED);
    set_window_flag_impl(WINDOW_SHOW_CURSOR);

    return 1;
}

u8 create_gl_context_impl(void) {
    if (!R3N_Windows_Internal.handle) return 0;    // error: window not yet created!

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

    int format = ChoosePixelFormat(R3N_Windows_Internal.deviceContext, &pfd);
    if (!SetPixelFormat(R3N_Windows_Internal.deviceContext, format, &pfd)) {
        return 0;  // error: failed to set pixel format!
    }

    R3N_Windows_Internal.glContext = wglCreateContext(R3N_Windows_Internal.deviceContext);
    if (!R3N_Windows_Internal.glContext) {
        return 0;  // error: failed to create GL context!
    }

    if (!wglMakeCurrent(R3N_Windows_Internal.deviceContext, R3N_Windows_Internal.glContext)) {
        wglDeleteContext(R3N_Windows_Internal.glContext);
        return 0;  // error: failed to make context current!
    }

    return 1;
}

void swap_buffers_impl(void) {
    if (!R3N_Windows_Internal.handle) return;    // error: no window created!
    SwapBuffers(R3N_Windows_Internal.deviceContext);
}

void destroy_gl_context_impl(void) {
    if (!R3N_Windows_Internal.handle) return;    // error: no window created!

    if (R3N_Windows_Internal.glContext) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(R3N_Windows_Internal.glContext);
        R3N_Windows_Internal.glContext = NULL;
    }
}

void destroy_window_impl(void) {
    if (!R3N_Windows_Internal.handle) return;    // error: window not yet created!
    DestroyWindow(R3N_Windows_Internal.handle);
    if (R3N_Windows_Internal.glContext) destroy_gl_context_impl();
}


void poll_events_impl(void) {
    MSG message;
    while(PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
}

void poll_inputs_impl(void) {
    R3N_Inputs_Ptr->update();
}


u8 load_library_impl(str path, str name, R3N_Library* library) {
    char full_path[MAX_PATH];
    if (path != NULL) {
        // TODO: levarage r3kt strings instead of snprintf!!!
        snprintf(full_path, sizeof(full_path), "%s/%s.dll", path, name);
    } else snprintf(full_path, sizeof(full_path), "%s.dll", name);
    
    library->handle = LoadLibrary(full_path);
    if (!library->handle) {
        r3_log_stdoutf(ERROR_LOG, "[R3N_Platform] failed to load library: %s\n", name);
        return 0; // error: failed to load library!
    }

    library->name = name;

    return 1;
}

u8 load_library_symbol_impl(str name, ptr* symbol, R3N_Library* library) {
    if (!library || !library->handle || !name) {
        r3_log_stdoutf(ERROR_LOG, "[R3N_Platform] failed to load symbol: %s\n", name);
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
        r3_log_stdoutf(ERROR_LOG, "[R3N_Platform] failed to load symbol: %s\n", name);
        return 0;
    } else {
        r3_log_stdoutf(INFO_LOG, "[R3N_Platform] loaded symbol: (name)%s (ptr)%p\n", name, *symbol);
        return 1;
    }
}

u8 unload_library_impl(R3N_Library* library) {
    if (!library || !library->handle) {
        r3_log_stdout(ERROR_LOG, "[R3N_Platform] invalid ptr :: unload_library_impl()\n");
        return 0;    // error: null ptr!
    }
    if (!FreeLibrary((HMODULE)library->handle)) {
        r3_log_stdout(ERROR_LOG, "[R3N_Platform] failed to free library\n");
        return 0;  // error: failed to free library!
    }

    library->name = NULL;
    library->handle = NULL;
    
    return 1;
}


u8 r3n_init_platform(none) {
    if (R3N_Events_Ptr == NULL) {
        r3_log_stdout(ERROR_LOG, "[R3N_Platform] R3N_Events not initialized!\n");
        return 0;
    }
    
    if (R3N_Inputs_Ptr == NULL) {
        r3_log_stdout(ERROR_LOG, "[R3N_Platform] R3N_Inputs not initialized!\n");
        return 0;
    }

    if (R3N_Platform_Ptr == NULL) {
        R3N_Platform_Ptr = r3_mem_alloc(sizeof(R3N_Platform), 8);
        if (R3N_Platform_Ptr == NULL) {
            r3_log_stdout(ERROR_LOG, "[R3N_Platform] failed to allocate R3N_Platform dispatch table!\n");
            return 0;
        }
    }

    R3N_Platform_Ptr->create_window = create_window_impl;
    R3N_Platform_Ptr->destroy_window = destroy_window_impl;
    
    R3N_Platform_Ptr->swap_buffers = swap_buffers_impl;
    R3N_Platform_Ptr->create_gl_context = create_gl_context_impl;
    R3N_Platform_Ptr->destroy_gl_context = destroy_gl_context_impl;
    
    R3N_Platform_Ptr->poll_events = poll_events_impl;
    R3N_Platform_Ptr->poll_inputs = poll_inputs_impl;
    
    R3N_Platform_Ptr->get_window = get_window_impl;
    R3N_Platform_Ptr->get_window_flag = get_window_flag_impl;
    R3N_Platform_Ptr->set_window_flag = set_window_flag_impl;
    R3N_Platform_Ptr->rem_window_flag = rem_window_flag_impl;
    
    R3N_Platform_Ptr->load_library = load_library_impl;
    R3N_Platform_Ptr->unload_library = unload_library_impl;
    R3N_Platform_Ptr->load_library_symbol = load_library_symbol_impl;

    r3_log_stdout(SUCCESS_LOG, "[R3N_Platform] initialized R3N_Platform\n");
    return 1;
}

u8 r3n_deinit_platform(none) {
    if (R3N_Platform_Ptr != NULL) {
        r3_mem_dealloc(R3N_Platform_Ptr);
        R3N_Platform_Ptr = NULL;
    }

    r3_log_stdout(SUCCESS_LOG, "[R3N_Platform] deinitialized R3N_Platform\n");
    return 1;
}

#endif  // R3N_PLATFORM_WINDOWS