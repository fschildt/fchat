#include "general.h"
#include <string.h>
#include <stdlib.h>

#include "platform/platform.h"
#include "renderer/renderer.h"

struct State;

enum State_Type {
    STATE_LOGIN,
    STATE_CHAT
};

static void state_change(struct State *state, enum State_Type type);

#include "login.h"
#include "chat.h"

#include "login.c"
#include "chat.c"

struct State {
    enum State_Type type;
    struct Login login;
    struct Chat chat;
};

static void state_change(struct State *state, enum State_Type type)
{
    state->type = type;
}

static void state_init(struct State *state)
{
    state->type = STATE_LOGIN;
    login_init(&state->login);
    chat_init(&state->chat);
}

static void render_state(struct State *state)
{
    if (state->type == STATE_LOGIN)
    {
        renderer_draw_color(0.7, 0.5, 0.3);
    }
    else
    {
        renderer_draw_color(0.3, 0.5, 0.7);
    }
}

int main(int argc, char **argv)
{
    if (!platform_init())
    {
        return 0;
    }

    struct Platform_Window *window = platform_create_window("fchat", 1280, 720);
    if (!window)
    {
        return 0;
    }

    if (!renderer_init())
    {
        return 0;
    }

#if 0
    struct File font_file;
    if (!platform_read_file(&font_file, "fonts/Inconsolata-Regular.ttf"))
    {
        return 0;
    }
#endif

    struct State state;
    state_init(&state);

    bool running = true;
    while (running)
    {
        // process window events
        struct Window_Event *window_event;
        while ((window_event = platform_get_window_event(window)))
        {
            if (state.type == STATE_LOGIN)
                login_process_window_event(&state.login, window_event);
            else
                chat_process_window_event(&state.chat, window_event);
        }

        // process network events
        if (state.login.flags & LOGIN_SUCCESS)
        {
            struct Platform_Connection *connection = state.login.connection;

            char buff[256];
            s32 bytes_received;
            while ((bytes_received = platform_receive(connection, buff, 256) > 0))
            {
                if (state.type == STATE_LOGIN)
                    login_process_network_event(&state, &state.login, buff, bytes_received);
                else
                    chat_process_network_event(&state.chat, (u8*)buff, bytes_received);
            }
            if (bytes_received < 0)
            {
                printf("connection over\n");
                running = false;
            }
        }

        // draw state
        render_state(&state);
        platform_swap_buffers(window);
    }

    return 0;
}

