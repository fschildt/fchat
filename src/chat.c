// NOTE: coordinates in pixels, origin top-left

// TODO:
// #include "stb_truetype.h"
// do all the quad calculations here too, arrange text in lines, draw those lines

static void draw_chat_prompt(struct Chat_Prompt *prompt, struct Rectangle rect)
{
    renderer_draw_text(prompt->input, rect.x, rect.y);
}

static void draw_chat_history(struct Chat_History *history, struct Rectangle rect)
{
    f32 xmax = rect.x + rect.width;
    f32 ymax = rect.y + rect.height;

    stbtt_bakedchar *baked_chars = renderer_get_stbtt_baked_chars();

    f32 y = ymax - 32;


    s32 base = history->base;
    s32 it = history->cnt - 1;
    while (it >= base)
    {
        f32 x = rect.x;

        struct Chat_Message *message = &history->messages[it];

        u32 name_len = strlen(message->name);
        u32 msg_len = strlen(message->msg);

        // 4: '<' , '>' , ' ' , '\0'
        u32 text_len = name_len + msg_len + 4;
        char text[text_len];

        text[0] = '<';
        memcpy(text+1, message->name, name_len);
        text[1+name_len] = '>';
        text[2+name_len] = ' ';

        memcpy(text+3+name_len, message->msg, msg_len);
        text[3+name_len+msg_len] = '\0';

        renderer_draw_text(text, x, y);

        y -= 32;
        it--;
    }
}

static void init_chat_rectangles(struct Rectangle *history, struct Rectangle *prompt, f32 xmin, f32 xmax, f32 ymin, f32 ymax)
{
    f32 width  = xmax - xmin;
    f32 height = ymax - ymin;

    f32 prompt_height  = 32;
    prompt->x = xmin;
    prompt->y = ymax - prompt_height;
    prompt->width  = width;
    prompt->height = prompt_height;

    history->x = xmin;
    history->y = ymin;
    history->width  = width;
    history->height = height - prompt_height;
}

static void draw_chat(struct Chat *chat, s32 window_width, s32 window_height)
{
    renderer_draw_color(0.5, 0.7, 0.3);

    struct Rectangle history_rect;
    struct Rectangle prompt_rect;
    f32 border = 100;
    f32 xmin = border;
    f32 xmax = window_width - 2*border;
    f32 ymin = border;
    f32 ymax = window_height - 2*border;
    init_chat_rectangles(&history_rect, &prompt_rect, xmin, xmax, ymin, ymax);

    draw_chat_prompt(&chat->prompt, prompt_rect);
    draw_chat_history(&chat->history, history_rect);
}

static void chat_process_network_event(struct Chat *chat, u8 *buff, u32 buff_size)
{
    if (buff_size < sizeof(u16))
    {
        printf("chat network event: buff too small\n");
        return;
    }
    u16 *type = (u16*)buff;
    printf("chat network event: type = %d\n", *type);
    if (*type == FCP_S_CHAT_MESSAGE)
    {
        if (buff_size < sizeof(struct FCP_S_Chat_Message_Desc))
        {
            printf("invalid message for FCP_Message_Desc\n");
            return;
        }

        struct FCP_S_Chat_Message_Desc *desc = (struct FCP_S_Chat_Message_Desc*)buff;
        if (buff_size < desc->message_len + sizeof(struct FCP_S_Chat_Message_Desc))
        {
            printf("recvd bytes not enough for message\n");
            return;
        }

        u32 descripted_size = sizeof(struct FCP_S_Chat_Message_Desc) + desc->sender_len + desc->message_len;
        if (descripted_size != buff_size)
        {
            printf("error descriptor wrong\n");
            return;
        }

        u8 *name = buff + sizeof(struct FCP_S_Chat_Message_Desc);
        u8 *msg  = name + desc->sender_len;
        struct Chat_History *history = &chat->history;

        // TODO: messages might not be in the right order yet
        u32 history_size = sizeof(history->messages)/sizeof(struct Chat_Message);
        if (history->base == -1)
        {
            memcpy(history->messages[0].name, name, desc->sender_len);
            memcpy(history->messages[0].msg,  msg,  desc->message_len);

            history->base = 0;
            history->cnt = 1;
        }
        else if (history->cnt < history_size)
        {
            s32 index = history->cnt;
            memcpy(history->messages[index].name, name, desc->sender_len);
            memcpy(history->messages[index].msg,  msg,  desc->message_len);

            history->cnt += 1;
        }
        else
        {
            s32 index = history->base;
            memcpy(history->messages[index].name, name, desc->sender_len);
            memcpy(history->messages[index].msg,  msg,  desc->message_len);

            if (index == 63)
                history->base = 0;
            else
                history->base += 1;
        }
        printf("history->cnt = %d\n", history->cnt);
    }
    else if (*type == FCP_S_ERROR)
    {
        printf("FCP_Message error\n");
    }
    else
    {
        printf("FCP_Message type not implemented yet\n");
    }
}

static void chat_process_window_event(struct Chat *chat, struct Window_Event *event, struct Platform_Connection *connection)
{
    u32 type = event->type;
    if (type == WINDOW_KEY)
    {
        struct Chat_Prompt *prompt = &chat->prompt;

        char ch = event->e.e_key.ch;
        if (ch >= 32 && ch <= 127)
        {
            // buffer full, ignore additional input
            if (prompt->len == sizeof(prompt->input)-1)
                return;

            prompt->input[prompt->len] = ch;
            prompt->len += 1;
            prompt->input[prompt->len] = '\0';
        }
        else if (ch == '\r')
        {
            struct FCP_C_Chat_Message_Desc desc;
            desc.type = FCP_C_CHAT_MESSAGE;
            desc.message_len = prompt->len;

            u32 package_size = sizeof(desc) + desc.message_len;
            u8 package[package_size];

            memcpy(package, &desc, sizeof(desc));
            memcpy(package+sizeof(desc), prompt->input, prompt->len);

            platform_send(connection, package, package_size);

            prompt->len = 0;
            prompt->input[0] = '\0';
        }
        else if (ch == 8)
        {
            if (prompt->len > 0)
            {
                prompt->len -= 1;
                prompt->input[prompt->len] = '\0';
            }
        }
        else
        {
            printf("unhandled character %d\n", ch);
        }
    }
    else
    {
        printf("TODO: handle chat_process_window_event\n");
    }
}

static void chat_init(struct Chat *chat)
{
    struct Chat_History *history = &chat->history;
    struct Chat_Prompt *prompt = &chat->prompt;

    history->base = 0;
    history->cnt = 0;
    memset(history->messages, 0, sizeof(history->messages)); // TODO: not neccessary

    prompt->len = 0;
    memset(prompt->input, 0, sizeof(prompt->input)); // TODO: not neccessary
}

