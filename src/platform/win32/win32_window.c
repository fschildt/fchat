#define WIN32_SERVICE_WINDOW_CREATED     (WM_USER + 0)
#define WIN32_SERVICE_WINDOW_NOT_CREATED (WM_USER + 1)
#define WIN32_CREATE_WINDOW              (WM_USER + 2)
#define WIN32_DESTROY_WINDOW             (WM_USER + 3)

// NOTE: SendMessage is synchronous, calls window_proc and returns

static PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
static HWND  g_service_window;
static DWORD g_main_thread_id;
static HMODULE g_opengl_module;

static void init_pfd(PIXELFORMATDESCRIPTOR *pfd)
{
    memset(pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
    pfd->nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd->nVersion = 1;
    pfd->dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd->iPixelType = PFD_TYPE_RGBA;
    pfd->cColorBits = 32;
    pfd->cDepthBits = 24;
    pfd->cStencilBits = 8;
    pfd->iLayerType = PFD_MAIN_PLANE;
}

static LRESULT window_proc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
    LRESULT result = 0;
    switch (message)
    {
        case WM_CLOSE:
        {
            PostThreadMessage(g_main_thread_id, message, (WPARAM)window, l_param);
        }
        break;

        case WM_CHAR:
        case WM_QUIT:
        case WM_SIZE:
        {
            PostThreadMessage(g_main_thread_id, message, w_param, l_param);
        }
        break;

        default:
        {
            result = DefWindowProcA(window, message, w_param, l_param);
        }
        break;
    }

    return result;
}

static HWND create_window(const char *name, int width, int height)
{
    const char *classname = "win32_window_class";

    WNDCLASSEX window_class = {};
    window_class.style = CS_OWNDC;
    window_class.cbSize = sizeof(window_class);
    window_class.lpfnWndProc = &window_proc;
    window_class.hInstance = GetModuleHandle(NULL);
    window_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    window_class.lpszClassName = classname;
    if (RegisterClassExA(&window_class) == 0)
    {
        printf("RegisterClassEx() failed\n");
        return 0;
    }

    HWND window = CreateWindowExA(0,
                                  classname,
                                  name,
                                  WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  width,
                                  height,
                                  0,
                                  0,
                                  window_class.hInstance,
                                  0);
    if (!window)
    {
        printf("CreateWindowEx() failed\n");
        return 0;
    }
    wglMakeCurrent(NULL, NULL);
    return window;
}

LRESULT win32_service_window_proc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
    printf("win32_service_window_proc message = %d\n", message);
	LRESULT result = 0;
	switch (message)
	{
        case WIN32_CREATE_WINDOW:
        {
            struct Platform_Window_Settings *settings = (struct Platform_Window_Settings*)w_param;
            return (LRESULT)create_window(settings->name, settings->width, settings->height);
        }
        break;

        case WIN32_DESTROY_WINDOW:
        {
            DestroyWindow(window);
        }
        break;

        default:
        {
            result = DefWindowProc(window, message, w_param, l_param);
        }
        break;
	}
	return result;
}

bool create_service_window()
{
    WNDCLASSEX service_window_class = {};
    service_window_class.style = CS_OWNDC;
    service_window_class.cbSize = sizeof(service_window_class);
    service_window_class.lpfnWndProc = &win32_service_window_proc;
    service_window_class.hInstance = GetModuleHandle(NULL);
    service_window_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    service_window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    service_window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    service_window_class.lpszClassName = "win32_service_window_class";
    if (RegisterClassEx(&service_window_class) == 0)
    {
        printf("RegisterClassEx() failed\n");
        return 0;
    }

    const char *service_window_name = "win32_service_window";
    HWND service_window = CreateWindowEx(0,
                                         service_window_class.lpszClassName,
                                         service_window_name,
                                         0,
                                         CW_USEDEFAULT,
                                         CW_USEDEFAULT,
                                         CW_USEDEFAULT,
                                         CW_USEDEFAULT,
                                         0,
                                         0,
                                         service_window_class.hInstance,
                                         0);
    if (!service_window)
    {
        printf("CreateWindowEx() failed\n");
        return 0;
    }

    /*
     * Create fake gl context, so I can create window with modern opengl context.
     * Doing this with the service window prevents creating another 'fake' window.
     */
    HDC dc = GetDC(service_window);
    if (!dc)
    {
        printf("GetDC() failed\n");
        return false;
    }

    PIXELFORMATDESCRIPTOR pfd;
    init_pfd(&pfd);

    int pixel_format = ChoosePixelFormat(dc, &pfd);
    if (pixel_format == 0)
    {
        printf("ChoosePixelFormat failed\n");
        return false;
    }

    BOOL pixel_format_set = SetPixelFormat(dc, pixel_format, &pfd);
    if (pixel_format_set == FALSE)
    {
        printf("SetPixelFormat() failed\n");
        return false;
    }

	HGLRC glrc = wglCreateContext(dc);
    if (!glrc)
    {
        printf("wglCreateContext() failed\n");
        return false;
    }

    BOOL made_current = wglMakeCurrent(dc, glrc);
    if (made_current == FALSE)
    {
        printf("wglMakeCurrent() failed\n");
        return false;
    }

    // TODO: check if extensions are actually supported
    // "WGL_ARB_pixel_format"
    // "WGL_ARB_create_context"

    // TODO: check all return values indicating invalid function from wglGetProcAddress
    // msdn: 0 https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-wglgetprocaddress
    // khronos: (void*){0,1,2,3,-1} https://www.khronos.org/opengl/wiki/Load_OpenGL_Functions#Windows_2
    wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
    if (!wglChoosePixelFormatARB || !wglCreateContextAttribsARB)
    {
        printf("wgl functions to create context not received\n");
        return false;
    }

    g_service_window = service_window;
    return true;
}

DWORD WINAPI win32_service_window_thread(LPVOID lpParameter)
{
    if (!create_service_window())
    {
        PostThreadMessage(g_main_thread_id, WIN32_SERVICE_WINDOW_NOT_CREATED, 0, 0);
        return 0;
    }

    PostThreadMessage(g_main_thread_id, WIN32_SERVICE_WINDOW_CREATED, 0, 0);

    for (;;)
    {
        // receive messages for all windows (created by this thread)
        MSG message;
        BOOL recvd = GetMessageA(&message, 0, 0, 0);
        if (recvd == -1)
        {
            // handle error
            printf("GetMessage failed\n");
            return 0;
        }

        // NOTE: DispatchMessage might only redirect message to the corresponding window
        //       but it also might do more than that, so I can't PostThreadMessage here
        TranslateMessage(&message); // translate virtual keycodes for character input
        DispatchMessage(&message);  // let windows call the window proc
    }
}

void platform_swap_buffers(struct Platform_Window *window)
{
    SwapBuffers(window->dc);
}

// NOTE: this only supports 1 Platform_Window, because the message queue is for all windows
//       and I can only process one message at a time, which might be from another window
struct Window_Event *platform_get_window_event(struct Platform_Window *window)
{
    static struct Window_Event window_event;

    MSG message;
    if (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
    {
        if (message.hwnd != window->window)
        {
            return 0;
        }

        switch (message.message)
        {
            case WM_SIZE:
            {
                window_event.type = WINDOW_RESIZE;
                window_event.e.e_resize.width  = LOWORD(message.lParam);
                window_event.e.e_resize.height = HIWORD(message.lParam);
                return &window_event;
            }
            break;

            case WM_CHAR:
            {
                printf("char %c pressed\n", (char)message.wParam);
                window_event.e.e_key.ch = (char)message.wParam;
                return &window_event;
            }
            break;

            default:
            {
                return 0;
            }
            break;
        }
    }

    return 0;
}

bool platform_destroy_window(struct Platform_Window *window)
{
    SendMessage(g_service_window, WIN32_DESTROY_WINDOW, (WPARAM)window->window, 0);
    return true;
}

struct Platform_Window* platform_create_window(const char *name, int width, int height)
{
    struct Platform_Window *window = malloc(sizeof(struct Platform_Window));
    if (!window)
    {
        printf("out of memory to create window\n");
        return 0;
    }

    // NOTE: SendMessage returns when execution is finished, so settings can be on stack
    struct Platform_Window_Settings settings = {name, width, height};
    HWND handle = (HWND)SendMessage(g_service_window, WIN32_CREATE_WINDOW, (WPARAM)&settings, 0);
    if (!handle)
        return 0;

    HDC dc = GetDC(handle);
    if (!dc)
    {
        printf("GetDC failed\n");
        return 0;
    }

    const int pixel_format_attribs[] =
    {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
        0, // End
    };

    int pixel_format;
    UINT cnt_pixel_formats;
    if (wglChoosePixelFormatARB(dc, pixel_format_attribs, NULL, 1, &pixel_format, &cnt_pixel_formats) == FALSE)
    {
        printf("wglChoosePixelFormat return false\n");
        return 0;
    }

    PIXELFORMATDESCRIPTOR pfd;
    init_pfd(&pfd);

    if (SetPixelFormat(dc, pixel_format, &pfd) == FALSE)
    {
        printf("SetPixelFormat return false\n");
        return 0;
    }

    int context_attribs[] = 
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    HGLRC glrc = wglCreateContextAttribsARB(dc, 0, context_attribs);
    if (!glrc)
    {
        printf("wglCreateContextAttribsARB failed\n");
        return 0;
    }

    if (wglMakeCurrent(dc, glrc) == FALSE)
    {
        printf("wglMakeCurrent failed\n");
        return 0;
    }

    window->window = handle;
    window->dc = dc;
    window->glrc = glrc;
    return window;
}

void* platform_get_gl_proc(const char *name)
{
    void* address = (void*)wglGetProcAddress(name);
    if (!address)
    {
        address = GetProcAddress(g_opengl_module, name);
    }
    return address;
}

bool platform_init()
{
    g_main_thread_id = GetCurrentThreadId();
    g_opengl_module = LoadLibraryA("opengl32.dll");
    if (!g_opengl_module)
    {
        printf("can't open opengl32.dll\n");
        return false;
    }

    DWORD tid;
    HANDLE thread = CreateThread(0, 0, win32_service_window_thread, 0, 0, &tid);
    if (!thread)
    {
        printf("error: CreateThread(...) failed\n");
        return false;
    }

    // wait until service window is ready
    for (;;)
    {
        MSG message;
        GetMessageA(&message, 0, 0, 0);

        if (message.message == WIN32_SERVICE_WINDOW_CREATED)
            return true;
        else
            return false;
    }
}
