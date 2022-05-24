// NOTE: window situation:
// win32 arbitrary blocks main thread with window massages (on resize)
// goal is to move create/destroy window stuff to another thread
// messages to the window are thread specific, they need to be caught there
// synchronize with win32 messaging system

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

