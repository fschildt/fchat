#include "../../general.h"
#include "../platform.h"

// general
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

// window
#include <X11/Xlib.h>
#include <GL/glx.h>
#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

// network
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

// threading
#include <pthread.h>

// i/o
#include <sys/stat.h>

// my program
#include "window.h"
#include "network.h"

#include "window.c"
#include "network.c"

char g_path_run_tree[256]; // should be big enough

void platform_free_file(struct File *file)
{
    free(file->buff);
}

bool platform_read_file(struct File *file, const char *pathname)
{
    int fd = open(pathname, O_RDONLY);
    if (fd == -1)
    {
        printf("error reading file %s\n", pathname);
        return false;
    }

    struct stat statbuff;
    if (fstat(fd, &statbuff) == -1)
    {
        printf("cant fstat file %s\n", pathname);
        close(fd);
        return false;
    }

    void *file_buff = malloc(statbuff.st_size);
    if (!file_buff)
    {
        printf("error: out of memory\n");
        close(fd);
        return false;
    }

    ssize_t bytes_read = read(fd, file_buff, statbuff.st_size);
    if (bytes_read != statbuff.st_size)
    {
        printf("error: only read %ld/%ld bytes from file %s\n", bytes_read, statbuff.st_size, pathname);
        close(fd);
        free(file_buff);
        return false;
    }

    file->size = statbuff.st_size;
    file->buff = file_buff;
    return true;
}

void platform_run_thread(void *(thread_runner)(void *_data), void *data)
{
    pthread_t thread;
    if (pthread_create(&thread, 0, thread_runner, data) != 0)
    {
        printf("failed to create thread\n");
        return;
    }
}

u32 platform_get_random_u32()
{
    u32 random = rand();
    return random;
}

bool
platform_init()
{
    // TODO: do this properly
    const char *path_run_tree = "/home/florian/dev/fchat/run_tree/";
    memcpy(g_path_run_tree, path_run_tree, strlen(path_run_tree)+1);

    srand(time(NULL)); // init random number generator

    return true;
}
