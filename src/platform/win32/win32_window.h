struct Platform_Window {
    HWND window;
    HDC dc;
    HGLRC glrc;
};

struct Platform_Window_Settings {
    const char *name;
    int width;
    int height;
};

