#include "../common_types.h"
#include "../program_options.h"
#include "../sakuren.h"
#include "../input.h"

#include "windows_with_undefs.h"

#include <GL/gl3w.h>
#include <GL/wglext.h>
// NOTE: extern-ed in sakuren.h
Platform* platform = 0;
Game* game = 0;

static HGLRC win32_opengl_context;
static HDC device_context;
static Platform platform_;

static HANDLE win32_stdout;

// NOTE: These WGL functions are needed to setup our OpenGL context
static PFNWGLCHOOSEPIXELFORMATARBPROC          wglChoosePixelFormatARB = NULL;
static PFNWGLCREATECONTEXTATTRIBSARBPROC       wglCreateContextAttribsARB = NULL;
static PFNWGLMAKECONTEXTCURRENTARBPROC  wglMakeContextCurrentARB = NULL;
static PFNWGLSWAPINTERVALEXTPROC               wglSwapIntervalEXT = NULL;

/* OpenGL 4.5 context */
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
		output_debug_string("[In win32_opengl_make_context()] pixel_format == 0 after ChoosePixelFormat()\n");
        return 0;
    }
    
    SetPixelFormat(device_context, pixel_format, &pfd);
    HGLRC test_render_context = wglCreateContext(device_context);
    wglMakeCurrent(device_context, test_render_context);

    // NOTE: Load the WGL ARB procs
    wglChoosePixelFormatARB     = (PFNWGLCHOOSEPIXELFORMATARBPROC)   wglGetProcAddress("wglChoosePixelFormatARB");
    wglCreateContextAttribsARB  = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
    wglMakeContextCurrentARB    = (PFNWGLMAKECONTEXTCURRENTARBPROC)  wglGetProcAddress("wglMakeContextCurrentARB");
    wglSwapIntervalEXT          = (PFNWGLSWAPINTERVALEXTPROC)        wglGetProcAddress("wglSwapIntervalEXT");

    // NOTE: Real pixel format
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

    // NOTE: Delete test render context
    wglMakeCurrent(device_context, 0);
    wglDeleteContext(test_render_context);
    wglMakeCurrent(device_context, win32_opengl_context);
    wglSwapIntervalEXT(VSYNC);  // NOTE: 1 to enable vsync, 0 for to disable vsync
    
    // NOTE: Load OpenGL core profile procs
    //       This just saves loads of busy work with OpenGL proc loading
    if (gl3wInit())
    {
        output_debug_string("[In win32_opengl_make_context()] gl3wInit() failed\n");
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
        
        // case WM_SIZE:
        //     {
        //         if (wparam == SIZE_MAXIMIZED)// || wparam == SIZE_RESTORED)  <-- SIZE_RESTORED breaks for some reason
        //         {
        //             platform_.window_width  = (int)(lparam & 0xFFFF);
        //             platform_.window_height = (int)(lparam & 0xFFFF0000);
        //             if (platform_.resize_callback != NULL)
        //             {
        //                 platform_.resize_callback(platform_.window_width, platform_.window_height);
        //             }
        //             // TEMP/DEBUG
        //             Game* game = (Game*)platform_.permanent_storage;
        //             output_debug_string(arena_push_string(&game->frame_arena, "Aspecta w/h: %f\n", (f32)platform_.window_width / (f32)platform_.window_height).data);
        //         }
        //     }
        //     break;
        
        // case WM_EXITSIZEMOVE:
        //     {
        //         RECT window_rect;
        //         GetClientRect(window, &window_rect);
        //         platform_.window_width = window_rect.right - window_rect.left;
        //         platform_.window_height = window_rect.bottom - window_rect.top;
        //         if (platform_.resize_callback != NULL)
        //         {
        //             platform_.resize_callback(platform_.window_width, platform_.window_height);
        //         }
        //         // TEMP/DEBUG: Output aspect ratio:
        //         Game* game = (Game*)platform_.permanent_storage;
        //         output_debug_string(arena_push_string(&game->frame_arena, "Aspect w/h: %f\n", (f32)platform_.window_width / (f32)platform_.window_height).data);
        //     }
        //     break;

        case WM_CAPTURECHANGED:
            {
                HWND captured_window = (HWND)lparam;
            }
            break;

        case WM_MOUSEMOVE:
            {
                Vector2i mouse_position = { S32_Low(lparam), S32_High(lparam) };
                platform_.input.mouse_relative = v2i_sub(mouse_position, platform_.last_input.mouse_position);
                platform_.input.mouse_position = mouse_position;
            }
            break;

        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
            {
                Vector2i mouse_click_position = { S32_Low(lparam), S32_High(lparam) };
                
                if (message == WM_LBUTTONDOWN)
                {
                    platform_.input.mouse_1_click_position = mouse_click_position;
                    platform_.input.mouse_1 = 1;
                }
                else if (message == WM_LBUTTONUP)
                {
                    platform_.input.mouse_1 = 0;
                }
                else if (message == WM_RBUTTONDOWN)
                {
                    platform_.input.mouse_2_click_position = mouse_click_position;
                    platform_.input.mouse_2 = 1;
                }
                else if (message == WM_RBUTTONUP)
                {
                    platform_.input.mouse_2 = 0;
                }
                else if (message == WM_MBUTTONDOWN)
                {
                    platform_.input.mouse_3 = 1;
                }
                else if (message == WM_MBUTTONUP)
                {
                    platform_.input.mouse_3 = 0;
                }
                // Mouse 4 and 5:
                else if (message == WM_XBUTTONDOWN)
                {
                    u32 button = GET_XBUTTON_WPARAM(wparam);

                    if (button == XBUTTON1)
                    {
                        platform_.input.mouse_4 = 1;
                    }
                    else if (button == XBUTTON2)
                    {
                        platform_.input.mouse_5 = 1;
                    }
                }
                else if (message == WM_XBUTTONUP)
                {
                    u32 button = GET_XBUTTON_WPARAM(wparam);

                    if (button == XBUTTON1)
                    {
                        platform_.input.mouse_4 = 0;
                    }
                    else if (button == XBUTTON2)
                    {
                        platform_.input.mouse_5 = 0;
                    }
                }
            }
            break;
        
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
            {
                int mod_alt = lparam & (1 << 29);
                platform_.input.mod_alt = mod_alt;

                if ( (wparam == VK_F4 && mod_alt) ||
                     (wparam == VK_ESCAPE) )
                {
                    PostQuitMessage(0);
                }

                if (message == WM_SYSKEYDOWN || message == WM_KEYDOWN)
                {
                    platform_.input.keys[wparam] = 1;
                }
                else if (message == WM_SYSKEYUP || message == WM_KEYUP)
                {
                    platform_.input.keys[wparam] = 0;
                }
            }
            break;

        default:
            result = DefWindowProcA(window, message, wparam, lparam);
    }

    return result;
}

int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{   
    // NOTE: Setup stdout console output
    win32_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (win32_stdout == NULL || win32_stdout == INVALID_HANDLE_VALUE)
    {
        report_error("GetStdHandle() failed");
    }

    // NOTE: Register window class
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
    window_class.lpszClassName = "Sakuren_Window_Class";
    window_class.hIconSm = NULL;

    if (!RegisterClassExA(&window_class))
    {
        MessageBoxA(NULL, "[Error]Failed to register window class", "Error", MB_OK | MB_ICONEXCLAMATION);
        ExitProcess(1);
    }
    
    DWORD window_style = WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    RECT window_rect = { 0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT };
    AdjustWindowRect(&window_rect, window_style, FALSE);  // NOTE: Calculates window size for the wanted client size and window style
    
    HWND window = CreateWindowExA(0, window_class.lpszClassName, WINDOW_TITLE, window_style, CW_USEDEFAULT, CW_USEDEFAULT, window_rect.right, window_rect.bottom, 0, 0, window_class.hInstance, 0);
    if (window == NULL)
    {
        MessageBoxA(NULL, "[Error]Failed to create window", "Error", MB_OK | MB_ICONEXCLAMATION);
        ExitProcess(1);
    }

    device_context = GetDC(window);

    if (!win32_opengl_make_context(device_context))
    {
        output_debug_string("[In WinMain()] win32_opengl_make_context() failed\n");
        return 1;
    }
    ShowCursor(1);

    // NOTE: Retrieve commandline arguments (lpCmdLine arg)
    // https://codingmisadventures.wordpress.com/2009/03/10/retrieving-command-line-parameters-from-winmain-in-win32/

    //
    // NOTE: Platform initialization
    platform_.permanent_storage_size = PERMANENT_STORAGE_SIZE;
    platform_.asset_storage_size     = ASSET_STORAGE_SIZE;
    platform_.transient_storage_size = TRANSIENT_STORAGE_SIZE;
    platform_.permanent_storage = VirtualAlloc(0, platform_.permanent_storage_size + platform_.asset_storage_size + platform_.transient_storage_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    platform_.asset_storage = (u8*)platform_.permanent_storage + platform_.permanent_storage_size;
    platform_.transient_storage = (u8*)platform_.permanent_storage + platform_.permanent_storage_size + platform_.asset_storage_size;
    // MAYBE: Seperate permanent and transient storage into 2 VirtualAlloc() calls

    platform_.window_title[0] = '\0';
    platform_.window_should_update_title = 0;
    platform_.window_width = DEFAULT_WINDOW_WIDTH;
    platform_.window_height = DEFAULT_WINDOW_HEIGHT;
    platform_.aspect_ratio = (f32)platform_.window_width / (f32)platform_.window_height;
    platform_.quit = 0;
    platform_.initialized = 0;

    platform_.fullscreen = FULLSCREEN;
    platform_.vsync = VSYNC;
    platform_.cursor_visible = 0;
    platform_.texture_filter = TEXTURE_FILTER;

    platform_.target_fps = TARGET_FPS;
    platform_.timems = 0;
    platform_.time = 0.0;
    platform_.previous_timems = 0;
    platform_.previous_time = 0.0;
    platform_.delta_timems = 0;
    platform_.delta_time = 0.0;

    platform_.input = (Input){ 0 };
    platform_.last_input = (Input){ 0 };
    //

    LARGE_INTEGER performance_counter_frequency;
    LARGE_INTEGER performance_counter_begin;
    LARGE_INTEGER performance_counter_end;

    // NOTE: Get the frequency of the performance counter once on initialization
    //       It is fixed at system boot and is consistent across all processors.
    QueryPerformanceFrequency((LARGE_INTEGER*)&performance_counter_frequency);  // NOTE: Never fails on WinXp or above

    // NOTE: Get Time before entering main loop, so we can see the time the program has been running
    //       instead of just the value of the performance counter since PC boot.
    LARGE_INTEGER performance_counter_at_program_begin;
    QueryPerformanceCounter(&performance_counter_at_program_begin);
    platform_.timems = (1000 * (performance_counter_end.QuadPart - performance_counter_at_program_begin.QuadPart)) / performance_counter_frequency.QuadPart;
    platform_.time = (f64)(performance_counter_end.QuadPart - performance_counter_at_program_begin.QuadPart) / (f64)performance_counter_frequency.QuadPart;
    
    while (!platform_.quit)
    {
        // NOTE: Get time at start of frame
        QueryPerformanceCounter(&performance_counter_begin);
        
        // NOTE: If client size does not match what we have stored in platform struct, update platform struct  and call resize_callback
        if (platform_.initialized)
        {
            // NOTE: Client rect is the size of the buffer we render to, so not including the window borders
            RECT window_rect;
            GetClientRect(window, &window_rect);
            if (platform_.window_width != window_rect.right - window_rect.left || platform_.window_height != window_rect.bottom - window_rect.top)
            {
                // NOTE: Update window size
                RECT window_rect;
                GetClientRect(window, &window_rect);
                platform_.window_width = window_rect.right - window_rect.left;
                platform_.window_height = window_rect.bottom - window_rect.top;
                platform_.aspect_ratio = (f32)platform_.window_width / (f32)platform_.window_height;
                if (platform_.resize_callback != NULL)
                {
                    platform_.resize_callback(platform_.window_width, platform_.window_height);
                }
                // TEMP/DEBUG: Output aspect ratio:
                Game* game = (Game*)platform_.permanent_storage;
                output_debug_string(arena_push_string(&game->frame_arena, "Aspect w/h: %f\n", (f32)platform_.window_width / (f32)platform_.window_height).data);
            }
        }

        // NOTE: Poll Win32 messages (message loop)
        MSG message;
        while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE))
        {
            if (message.message == WM_QUIT)
            {
                ExitProcess(message.wParam);
            }
            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        b32 last_fullscreen = platform_.fullscreen;
        b32 last_vsync = platform_.vsync;
        b32 last_cursor_visible = platform_.cursor_visible;

        // NOTE: Update and render
        game_update_and_render(&platform_);
        
        // NOTE: Check toggle variables
        if (platform_.window_should_update_title)
        {
            SetWindowTextA(window, platform_.window_title);
            platform_.window_should_update_title = 0;
        }
        
        if (last_fullscreen != platform_.fullscreen)
        {
            win32_toggle_fullscreen(window);
        }
        if (last_vsync != platform_.vsync)
        {
            wglSwapIntervalEXT(platform_.vsync);
        }
        if (last_cursor_visible != platform_.cursor_visible)
        {
            ShowCursor(platform_.cursor_visible);
        }

        if (platform_.last_input.mouse_hidden != platform_.input.mouse_hidden)
        {
            ShowCursor(!platform_.input.mouse_hidden);
        }

        // NOTE: Finally update platform_.last_input
        platform_.last_input = platform_.input;
        
        // NOTE: Update timers
        platform_.previous_timems = platform_.timems;
        platform_.previous_time = platform_.time;

        QueryPerformanceCounter(&performance_counter_end);  // NOTE: Get time at end of frame
        s64 performance_counter_elapsed = performance_counter_end.QuadPart - performance_counter_begin.QuadPart;
        f64 time_for_this_update = performance_counter_elapsed / performance_counter_frequency.QuadPart;

        // NOTE: Sleep remainder of frame if needed
        s64 target_seconds_per_frame = 1.0f / (f64)platform_.target_fps;
        if (time_for_this_update  < target_seconds_per_frame)
        {
            Sleep(target_seconds_per_frame - time_for_this_update);
        }
        else
        {
            // NOTE: Missed frame
        }

        LARGE_INTEGER performance_counter_after_sleep;
        QueryPerformanceCounter(&performance_counter_after_sleep);
        platform_.time = (f64)(performance_counter_after_sleep.QuadPart - performance_counter_at_program_begin.QuadPart) / (f64)performance_counter_frequency.QuadPart;
        platform_.timems = 1000 * platform_.time;

        platform_.delta_time = (f64)(performance_counter_after_sleep.QuadPart - performance_counter_begin.QuadPart) / (f64)performance_counter_frequency.QuadPart;
        platform_.delta_timems = 1000 * platform_.delta_time;
    }
    
    return 0;
}

/////////////////////////////////////////////////////////////
// NOTE: Win32 implementations of cross-platform functions //
/////////////////////////////////////////////////////////////

void*
DEBUG_read_entire_file(char* filename)
{
    void* result = 0;

    HANDLE file_handle = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (file_handle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER file_size;
        if(GetFileSizeEx(file_handle, &file_size))
        {
            u32 file_size32 = safe_truncate_u64(file_size.QuadPart);
            result = VirtualAlloc(0, file_size.QuadPart, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            if (result)
            {
                DWORD bytes_read;
                if (ReadFile(file_handle, result, file_size.QuadPart, &bytes_read, 0) && (file_size32 == bytes_read))
                {
                    // NOTE: File read successfully
                }
                else
                {
                    DEBUG_free_file_memory(result);
                    result = 0;
                }
            }
            else
            {
                // TODO:
            }
            
        }
        else
        {
            // TODO:
        }

        CloseHandle(file_handle);
    }
    else
    {
        // TODO:
    }
    

    return result;
}

void
DEBUG_free_file_memory(void* memory)
{
    if (memory)
    {
        VirtualFree(memory, 0, MEM_RELEASE);
    }
}

b32
DEBUG_write_entire_file(char* filename, u32 size, void* memory)
{
    HANDLE file_handle = CreateFileA(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file_handle != INVALID_HANDLE_VALUE)
    {
        DWORD bytes_written;
        if (WriteFile(file_handle, memory, size, &bytes_written, NULL) && size == bytes_written)
        {
            // NOTE: File wrote successfully
        }
        else
        {
            // NOTE: Failed to write to file
            CloseHandle(file_handle);
            return 0;
        }

        CloseHandle(file_handle);
    }
    else
    {
        // NOTE: Failed to create file
        return 0;
    }
    
    return 1;
}


void
set_window_title(char* text)
{
    // NOTE: Copy the text into the window_title buffer incase text is zero-ed
    u32 text_length = cstring_length(text);
    size_t text_size = Min(text_length, WINDOW_TITLE_MAX_CHARS);
    if (text_size < text_length)
    {
        text[WINDOW_TITLE_MAX_CHARS - 1] = '\0';  // NOTE: Truncate string with null terminator
    }
    
    copy_memory(text, text_size, platform_.window_title);
    platform_.window_should_update_title = 1;
}

char*
get_window_title()
{
    return platform_.window_title;
}

void
output_debug_string(char* text)
{
    // OutputDebugStringA((LPCSTR)text);
    DWORD written = 0;
    WriteConsoleA(win32_stdout, text, lstrlenA(text), &written, NULL);
}

void
report_error(char* text)
{
    MessageBoxA(NULL, text, "report_error():", MB_ICONEXCLAMATION);
}

void
swap_buffers()
{
    wglSwapLayerBuffers(device_context, WGL_SWAP_MAIN_PLANE);
}
