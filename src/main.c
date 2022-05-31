#include "general.h"
#include <string.h>
#include <stdlib.h>

#include "external/stb_truetype.h"
#include "platform/platform.h"
#include "renderer/renderer.h"

struct State;

enum State_Type {
    STATE_LOGIN,
    STATE_CHAT
};

static void state_change(struct State *state, enum State_Type type);

#include "external/fcp/fcp.h"
#include "login.h"
#include "chat.h"

#include "login.c"
#include "chat.c"

struct Assets {
    struct File font;
    struct File font_vs;
    struct File font_fs;
};

struct State {
    struct Assets assets;
    enum State_Type type;
    struct Login login;
    struct Chat chat;
};

static void renderer_draw_state(struct State *state, s32 window_width, s32 window_height)
{
    if (state->type == STATE_LOGIN)
    {
        struct Login *l = &state->login;
        renderer_draw_color(0.7, 0.5, 0.3);
        renderer_draw_text(l->values[0], 200, 200);
        renderer_draw_text(l->values[1], 200, 400);
        renderer_draw_text(l->values[2], 200, 600);
    }
    else if (state->type == STATE_CHAT)
    {
        struct Chat *chat = &state->chat;
        draw_chat(chat, window_width, window_height);
    }
    else
    {
        renderer_draw_text("illegal state", 200, 200);
    }
}

static bool load_assets(struct Assets *assets)
{
    if (!platform_read_file(&assets->font, "./fonts/cruft/cruft.ttf") ||
        !platform_read_file(&assets->font_vs, "./shader/font.vs") ||
        !platform_read_file(&assets->font_fs, "./shader/font.fs"))
    {
        return false;
    }

    return true;
}

static void state_change(struct State *state, enum State_Type type)
{
    state->type = type;
}

static bool state_init(struct State *state)
{
    if (!load_assets(&state->assets))
        return false;

    state->type = STATE_LOGIN;
    login_init(&state->login);
    chat_init(&state->chat);

    return true;
}

int main(int argc, char **argv)
{
    struct Platform_Window *window;
    struct State state;

    if (!platform_init() ||
        !(window = platform_create_window("fchat", 1280, 720)) ||
        !renderer_init() ||
        !state_init(&state))
    {
        return 0;
    }

    renderer_setup_text_drawing(state.assets.font.buff, state.assets.font_vs.buff, state.assets.font_fs.buff);

    s32 window_width = 0;
    s32 window_height = 0;

    bool running = true;
    while (running)
    {
        // process window events
        struct Window_Event *window_event;
        while ((window_event = platform_get_window_event(window)))
        {
            if (window_event->type == WINDOW_RESIZE)
            {
                window_width  = window_event->e.e_resize.width;
                window_height = window_event->e.e_resize.height;
                renderer_viewport(0, 0, window_width, window_height);
                continue;
            }

            if (state.type == STATE_LOGIN)
            {
                printf("login: processing window event\n");
                login_process_window_event(&state.login, window_event);
            }
            else
            {
                printf("chat: processing window event\n");
                chat_process_window_event(&state.chat, window_event, state.login.connection);
            }
        }

        // process network events
        if (state.login.flags & (LOGIN_SUCCESS | LOGIN_WAITING))
        {
            struct Platform_Connection *connection = state.login.connection;

            s32 bytes_received;
            u8 buff[256];
            while (((bytes_received = platform_receive(connection, buff, 256)) > 0))
            {
                printf("bytes_received = %d\n", bytes_received);
                if (state.type == STATE_LOGIN)
                    login_process_network_event(&state, &state.login, buff, bytes_received);
                else
                    chat_process_network_event(&state.chat, buff, bytes_received);
            }
            if (bytes_received < 0)
            {
                printf("connection over\n");
                running = false;
            }
            if (bytes_received == 0)
            {
                printf("bytes_received = 0\n");
            }
        }

        // draw state
        renderer_draw_state(&state, window_width, window_height);
        platform_swap_buffers(window);
    }

    return 0;
}

