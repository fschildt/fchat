// TODO: more informative error messages, use GetLastError

#include "../platform.h"
#include <winsock2.h>
#include <Ws2tcpip.h>
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
    HANDLE file_handle = CreateFile(pathname, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (file_handle == INVALID_HANDLE_VALUE)
    {
        printf("CreateFile failed\n");
        return false;
    }

    LARGE_INTEGER file_size;
    if (GetFileSizeEx(file_handle, &file_size) == INVALID_FILE_SIZE)
    {
        printf("GetFileSizeEx failed\n");
        CloseHandle(file_handle);
        return false;
    }

    void *buff = VirtualAlloc(0, file_size.QuadPart, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    if (!buff)
    {
        printf("out of memory\n");
        CloseHandle(file_handle);
        return false;
    }

    DWORD bytes_read;
    if (ReadFile(file_handle, buff, file_size.QuadPart, &bytes_read, 0) == 0 ||
        bytes_read != file_size.QuadPart)
    {
        printf("ReadFile failed\n");
        VirtualFree(buff, 0, MEM_RELEASE);
        CloseHandle(file_handle);
        return false;
    }

    file->buff = buff;
    file->size = bytes_read;
    return true;
}

void platform_run_thread(void *(thread_runner)(void *_data), void *data)
{
}

u32 platform_get_random_u32()
{
    return 0;
}


