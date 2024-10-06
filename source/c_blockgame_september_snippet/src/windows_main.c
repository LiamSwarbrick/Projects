#include "program.h"

#include <GL/GL3w.h>
#include <GL/wglext.h>
#include <stdio.h>

Program program;
Input input, last_input;


static HGLRC win32_opengl_context;
static int is_opengl_initialised = 0;
static HDC device_context;

static PFNWGLCHOOSEPIXELFORMATARBPROC          wglChoosePixelFormatARB      = NULL;
static PFNWGLCREATECONTEXTATTRIBSARBPROC       wglCreateContextAttribsARB   = NULL;
static PFNWGLMAKECONTEXTCURRENTARBPROC         wglMakeContextCurrentARB     = NULL;
static PFNWGLSWAPINTERVALEXTPROC               wglSwapIntervalEXT           = NULL;

static LRESULT CALLBACK     win32_window_callback(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
static int                  win32_opengl_make_context(HDC device_context);
static void                 win32_toggle_fullscreen(HWND window);

int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    program.w = WINDOW_INITIAL_W;
    program.h = WINDOW_INITIAL_H;

    // Create window
    {
        WNDCLASSEXA window_class;
        ZeroMemory(&window_class, sizeof(window_class));
        window_class.cbSize = sizeof(window_class);
        window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        window_class.lpfnWndProc = win32_window_callback;
        window_class.cbClsExtra = 0;
        window_class.cbWndExtra = 0;
        window_class.hInstance = GetModuleHandleA(NULL);
        window_class.hIcon = NULL;
        window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
        window_class.hbrBackground = NULL;
        window_class.lpszMenuName = NULL;
        window_class.lpszClassName = "Liam's OpenGL C program hell yeah!";
        window_class.hIconSm = NULL;

        if (!RegisterClassExA(&window_class))
        {
            MessageBoxA(NULL, "[Error]Failed to register window class", "Error", MB_OK | MB_ICONEXCLAMATION);
            ExitProcess(1);
        }

        DWORD window_style = WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;// | WS_VSCROLL;

        RECT window_rect = { 0, 0, program.w, program.h };
        AdjustWindowRectEx(&window_rect, window_style, FALSE, 0);  // Calculate required size of window, based on desired size of the client rectangle (window_rect).

        HWND window = CreateWindowExA(0, window_class.lpszClassName, WINDOW_TITLE, window_style, CW_USEDEFAULT, CW_USEDEFAULT, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top, NULL, NULL, window_class.hInstance, 0);
        if (window == NULL)
        {
            MessageBoxA(NULL, "[Error]Failed to create window", "Error", MB_OK | MB_ICONEXCLAMATION);
            ExitProcess(1);
        }

        device_context = GetDC(window);
        if (!win32_opengl_make_context(device_context))
        {
            MessageBoxA(NULL, "[Error]Failed to make OpenGL context.", "Error", MB_OK | MB_ICONEXCLAMATION);
            ExitProcess(1);
        }
        is_opengl_initialised = 1;

        // Find monitor refresh rate
        DEVMODEA devmode = { 0 };
        if (!EnumDisplaySettingsA(NULL, ENUM_CURRENT_SETTINGS, &devmode))
        {
            MessageBoxA(NULL, "[Error]Failed to retreive monitor settings.", "Error", MB_OK | MB_ICONEXCLAMATION);
            ExitProcess(1);
        }

        program.target_fps = devmode.dmDisplayFrequency;
        program.window_handle = window;
    }

    // Set FPS cam cursor pivot
    {
        input.mouse_fps_pivot_x = program.w / 2;
        input.mouse_fps_pivot_y = program.h / 2;

        // Convert window coordinate to screen coordinates
        POINT client_to_screen_point = { input.mouse_fps_pivot_x, input.mouse_fps_pivot_y};
        ClientToScreen(program.window_handle, &client_to_screen_point);
        input.mouse_fps_pivot_x = client_to_screen_point.x;
        input.mouse_fps_pivot_y = client_to_screen_point.y;
    }

    // Initialise timers
    LARGE_INTEGER performance_counter_frequency;
    LARGE_INTEGER performance_counter_begin;
    LARGE_INTEGER performance_counter_end;
    LARGE_INTEGER performance_counter_at_program_begin;
    {
        // Get the frequency of the performance counter once on initialization.
        // It is fixed at system boot and is consistent across all processors.
        QueryPerformanceFrequency((LARGE_INTEGER*)&performance_counter_frequency);
        QueryPerformanceCounter(&performance_counter_at_program_begin);
        program.time = (f64)(performance_counter_end.QuadPart - performance_counter_at_program_begin.QuadPart) / (f64)performance_counter_frequency.QuadPart;
    }

    // Main loop
    int exit_code = 0;
    for (u64 frame_count = 0;; ++frame_count)
    {
        QueryPerformanceCounter(&performance_counter_begin);

        // Event loop
        {
            // Poll Win32 messages (message loop)
            MSG message;
            while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE))
            {
                if (message.message == WM_QUIT)
                {
                    // ExitProcess(message.wParam);
                    exit_code = message.wParam;
                    goto MAINLOOP_END;
                }
                TranslateMessage(&message);
                DispatchMessage(&message);
            }

            // Get mouse position
            {
                POINT cursor_pos;
                GetCursorPos(&cursor_pos);

                input.mouse_pos_x = cursor_pos.x;
                input.mouse_pos_y = cursor_pos.y;

                if (input.mouse_fps_mode)
                {
                    input.mouse_relative_x = input.mouse_pos_x - input.mouse_fps_pivot_x;
                    input.mouse_relative_y = input.mouse_pos_y - input.mouse_fps_pivot_y;
                    SetCursorPos(input.mouse_fps_pivot_x, input.mouse_fps_pivot_y);  // Recentre cursor
                }
                else
                {
                    input.mouse_relative_x = 0;
                    input.mouse_relative_y = 0;
                }
            }
        }
        
        if (frame_count == 0)
        {
            init();
        }

        update_and_render();
        wglSwapLayerBuffers(device_context, WGL_SWAP_MAIN_PLANE);

        // Update last_input
        last_input = input;

        // Frame timing
        {
            // Get frame time
            QueryPerformanceCounter(&performance_counter_end);
            s64 performance_counter_elapsed = performance_counter_end.QuadPart - performance_counter_begin.QuadPart;
            f64 time_elapsed_this_update = performance_counter_elapsed / performance_counter_frequency.QuadPart;

            // Sleep remainder of frame if needed
            s64 target_seconds_per_frame = 1.0f / (f64)program.target_fps;
            if (time_elapsed_this_update < target_seconds_per_frame)
            {
                Sleep(target_seconds_per_frame - time_elapsed_this_update);
            }
            else
            {
                // Missed frame
            }

            // Set program timer variables
            LARGE_INTEGER performance_counter_after_sleep;
            QueryPerformanceCounter(&performance_counter_after_sleep);
            program.time = (f64)(performance_counter_after_sleep.QuadPart - performance_counter_at_program_begin.QuadPart) / (f64)performance_counter_frequency.QuadPart;
            program.dt = (f32)((f64)(performance_counter_after_sleep.QuadPart - performance_counter_begin.QuadPart) / (f64)performance_counter_frequency.QuadPart);
        }
    }
MAINLOOP_END:

    return exit_code;
}

static LRESULT CALLBACK
win32_window_callback(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    LRESULT result = 0;

    switch (message)
    {
        case WM_CLOSE:
            {
                DestroyWindow(window);
            }
            break;
        
        case WM_DESTROY:
            {
                PostQuitMessage(0);
            }
            break;
        
        case WM_SIZE:
            {
                RECT new_client_rect = { 0 };
                GetClientRect(window, &new_client_rect);
                program.w = new_client_rect.right - new_client_rect.left;
                program.h = new_client_rect.bottom - new_client_rect.top;
                program.aspect_ratio = (f32)program.w / (f32)program.h;
                
                if (is_opengl_initialised)
                    glViewport(0, 0, program.w, program.h);
                
                // Set FPS cam cursor pivot
                {
                    input.mouse_fps_pivot_x = program.w / 2;
                    input.mouse_fps_pivot_y = program.h / 2;

                    // Convert window coordinate to screen coordinates
                    POINT client_to_screen_point = { input.mouse_fps_pivot_x, input.mouse_fps_pivot_y};
                    ClientToScreen(window, &client_to_screen_point);
                    input.mouse_fps_pivot_x = client_to_screen_point.x;
                    input.mouse_fps_pivot_y = client_to_screen_point.y;
                }
            }
            break;
        
        case WM_MOVE:
        {
            // Get new position of window
            int new_window_x = LOWORD(lparam);
            int new_window_y = HIWORD(lparam);

            // Set FPS cam cursor pivot
            input.mouse_fps_pivot_x = new_window_x + (program.w / 2);
            input.mouse_fps_pivot_y = new_window_y + (program.h / 2);
        }
        break;


        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
            {
                int mod_alt = lparam & (1 << 29);

                if ( (wparam == VK_F4 && mod_alt) ||
                     (wparam == VK_ESCAPE) )
                {
                    PostQuitMessage(0);
                }
                
                if (message == WM_SYSKEYDOWN || message == WM_KEYDOWN)
                {
                    input.keys[wparam] = 1;
                }
                else if (message == WM_SYSKEYUP || message == WM_KEYUP)
                {
                    input.keys[wparam] = 0;
                }

            }
            break;


        default:
             result = DefWindowProcA(window, message, wparam, lparam);
    }


    return result;
}

static int
win32_opengl_make_context(HDC device_context)
{
    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize       = sizeof(pfd);
    pfd.nVersion    = 1;
    pfd.dwFlags     = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType  = PFD_TYPE_RGBA;
    pfd.cColorBits  = 32;
    pfd.cAlphaBits  = 8;
    pfd.cDepthBits  = 24;

    int pixel_format = ChoosePixelFormat(device_context, &pfd);
    if (pixel_format == 0)
    {
		printf("[In win32_opengl_make_context()] pixel_format == 0 after ChoosePixelFormat()\n");
        return 0;
    }
    
    SetPixelFormat(device_context, pixel_format, &pfd);
    HGLRC test_render_context = wglCreateContext(device_context);
    wglMakeCurrent(device_context, test_render_context);

    // Load the WGL ARB procs
    wglChoosePixelFormatARB     = (PFNWGLCHOOSEPIXELFORMATARBPROC)   wglGetProcAddress("wglChoosePixelFormatARB");
    wglCreateContextAttribsARB  = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
    wglMakeContextCurrentARB    = (PFNWGLMAKECONTEXTCURRENTARBPROC)  wglGetProcAddress("wglMakeContextCurrentARB");
    wglSwapIntervalEXT          = (PFNWGLSWAPINTERVALEXTPROC)        wglGetProcAddress("wglSwapIntervalEXT");

    // Real pixel format
    // Now we can request multisampling
    const int pixel_format_attribs[] = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_ALPHA_BITS_ARB, 8,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
        WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
        WGL_SAMPLES_ARB, 4,
        0
    };
    
    UINT num_formats = 0;
    wglChoosePixelFormatARB(device_context, pixel_format_attribs, NULL, 1, &pixel_format, &num_formats);
    if (pixel_format == 0)
    {
        return 0;
    }

    const int context_attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,  // OpenGL 4.5 Core Profile
        WGL_CONTEXT_MINOR_VERSION_ARB, 5,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
        0
    };

    win32_opengl_context = wglCreateContextAttribsARB(device_context, test_render_context, context_attribs);
    if (win32_opengl_context == NULL)
    {
        return 0;
    }

    // Delete test render context
    wglMakeCurrent(device_context, 0);
    wglDeleteContext(test_render_context);
    wglMakeCurrent(device_context, win32_opengl_context);
    wglSwapIntervalEXT(1);  // VSYNC enabled; 1 means swap buffer every 1 video frame periods.
    
    // Load OpenGL core profile procs
    //       This just saves loads of busy work with OpenGL proc loading
    if (gl3wInit())
    {
        printf("[In win32_opengl_make_context()] gl3wInit() failed\n");
        return 0;
    }

    return 1;
}

static void
win32_toggle_fullscreen(HWND window)
{
    static WINDOWPLACEMENT last_window_placement = { 0 };
    last_window_placement.length = sizeof(last_window_placement);

    DWORD window_style = GetWindowLong(window, GWL_STYLE);
    if (window_style & WS_OVERLAPPEDWINDOW)
    {
        MONITORINFO monitor_info = { 0 };
        monitor_info.cbSize = sizeof(monitor_info);
        if (GetWindowPlacement(window, &last_window_placement) && GetMonitorInfoA(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &monitor_info))
        {
            SetWindowLong(window, GWL_STYLE, window_style & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(window, HWND_TOP,
                         monitor_info.rcMonitor.left,
                         monitor_info.rcMonitor.top,
                         monitor_info.rcMonitor.right -
                         monitor_info.rcMonitor.left,
                         monitor_info.rcMonitor.bottom -
                         monitor_info.rcMonitor.top,
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    }
    else
    {
        SetWindowLong(window, GWL_STYLE,
                      window_style | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(window, &last_window_placement);
        SetWindowPos(window, 0, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                     SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}
