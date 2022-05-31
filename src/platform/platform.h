#ifndef PLATFORM_H
#define PLATFORM_H

#include "../general.h"

struct Platform_Window;
struct Platform_Connection;

/*
 * window
 */

enum Window_Event_Type {
    WINDOW_IGNORE,
    WINDOW_RESIZE,
    WINDOW_KEY
};

struct Window_Resize {
    s32 width;
    s32 height;
};

struct Window_Key {
    char ch;
};

struct Window_Event {
    enum Window_Event_Type type;
    union {
        struct Window_Resize e_resize;
        struct Window_Key    e_key;
    } e;
};


struct Platform_Window *platform_create_window(const char *name, int width, int height);
bool platform_destroy_window(struct Platform_Window *window);
struct Window_Event *platform_get_window_event(struct Platform_Window *window);
void platform_swap_buffers(struct Platform_Window *window);
void *platform_get_gl_proc(const char *name);


/*
 * connection
 */
struct Platform_Connection* platform_connect_to_server(const char *address, u16 port);
void platform_disconnect_from_server(struct Platform_Connection *connection);
bool platform_send(struct Platform_Connection *connection, void *buffer, u64 size);
s32 platform_receive(struct Platform_Connection *connection, void *buffer, u64 size);


/*
 * I/O
 */
struct File {
    void *buff;
    u32 size;
};
bool platform_read_file(struct File *file, const char *pathname);
void platform_free_file(struct File *file);


/*
 * thread
 */
void platform_run_thread(void *(thread_runner)(void *_data), void *data);

/*
 * misc
 */
u32 platform_get_random_u32();

bool platform_init();

#endif // PLATFROM_H

