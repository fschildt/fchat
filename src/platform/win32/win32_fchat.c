// TODO: more informative error messages, use GetLastError

#include "../platform.h"
#include <windows.h>

#include <GL/gl.h>
#include "../../external/khronos/wglext.h"

#include "win32_window.h"
#include "win32_network.h"

#include "win32_window.c"
#include "win32_network.c"

#include <stdio.h>

bool platform_read_file(struct File *file, const char *pathname)
{
    return false;
}

void platform_run_thread(void *(thread_runner)(void *_data), void *data)
{
}

u32 platform_get_random_u32()
{
    return 0;
}
