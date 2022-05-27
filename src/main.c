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

struct Assets {
    struct File font_buff;
    struct File font_vs;
    struct File font_fs;
};

struct State {
    struct Assets assets;
    enum State_Type type;
    struct Login login;
    struct Chat chat;
};

static void render_state(struct State *state)
{
    if (state->type == STATE_LOGIN)
    {
        struct Login *l = &state->login;
        renderer_draw_color(0.7, 0.5, 0.3);
        renderer_draw_text(l->values[0], 200, 200);
        renderer_draw_text(l->values[1], 200, 400);
        renderer_draw_text(l->values[2], 200, 600);
    }
    else
    {
        renderer_draw_color(0.3, 0.5, 0.7);
    }
}

static bool load_assets(struct Assets *assets)
{
    if (!platform_read_file(&assets->font_buff, "./fonts/cruft/cruft.ttf") ||
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

    renderer_setup_text_drawing(state.assets.font_buff.buff, state.assets.font_vs.buff, state.assets.font_fs.buff);

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

