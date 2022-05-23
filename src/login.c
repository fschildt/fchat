static void login_process_network_event(struct State *state, struct Login *login, const char *buff, u32 buff_len)
{
    if (!(login->flags & LOGIN_WAITING))
    {
        printf("invalid network event in login");
        return;
    }

    if (buff_len != 1 || buff[0] == 0)
    {
        printf("invalid server message in login");
        return;
    }
    else
    {
        printf("changing state from login to chat\n");
        login->flags ^= LOGIN_WAITING;
        login->flags |= LOGIN_SUCCESS;
        state_change(state, STATE_CHAT);
    }
}

static void login_process_window_event(struct Login *login, struct Window_Event *event)
{
    enum Window_Event_Type type = event->type;

    if (type == WINDOW_RESIZE)
    {
        s32 width  = event->e.e_resize.width;
        s32 height = event->e.e_resize.height;
        renderer_viewport(0, 0, width, height);
    }
    else if (type == WINDOW_KEY)
    {
        char ch = event->e.e_key.ch;

        if (login->flags & LOGIN_WAITING)
        {
            printf("ignoring key while waiting for login");
        }
        else
        {
            if (ch == '\r')
            {
                char *addr = login->values[LOGIN_INDEX_ADDR];
                char *port = login->values[LOGIN_INDEX_PORT];
                u16 addr_len = login->lengths[LOGIN_INDEX_ADDR];
                u16 port_len = login->lengths[LOGIN_INDEX_PORT];

                addr[addr_len] = '\0';
                port[port_len] = '\0';
                u16 port_real = atoi(port);

                struct Platform_Connection *connection = platform_connect_to_server(addr, port_real);
                if (!connection)
                {
                    login->flags |= LOGIN_FAILED;
                    return;
                }
                login->connection = connection;

                char *name   = login->values[LOGIN_INDEX_NAME];
                u16 name_len = login->lengths[LOGIN_INDEX_NAME];
                platform_send(login->connection, name, name_len);

                login->flags |= LOGIN_WAITING;
                printf("login now waiting for server\n");
            }
            else if (ch == '\t')
            {
                login->index++;
                if (login->index == LOGIN_CNT_INDICES)
                    login->index = LOGIN_INDEX_ADDR;
                printf("changing tab to %d\n", login->index);
            }
            else if (ch == ' ')
            {
                char *val   = login->values[login->index];
                u16 val_len = login->lengths[login->index];
                val[val_len] = '\0';
                printf("value(%d) = %s\n", login->index, val);
            }
            else if (ch == 8)
            {
                u16 val_len = login->lengths[login->index];
                if (val_len > 0)
                    login->lengths[login->index] -= 1;
            }
            else
            {
                printf("editing tab\n");
                char *value  = login->values[login->index];
                u16  *value_len = &(login->lengths[login->index]);

                value[*value_len] = ch;
                *value_len += 1;
            }
        }
    }
}

static void login_init(struct Login *login)
{
    login->connection = 0;
    login->flags = 0;
    login->index = LOGIN_INDEX_NAME;

    char addr[] = "127.0.0.1";
    char port[] = "3232";
    u16 addr_len = strlen(addr);
    u16 port_len = strlen(port);

    memcpy(login->values[LOGIN_INDEX_ADDR], addr, addr_len);
    memcpy(login->values[LOGIN_INDEX_PORT], port, port_len);

    login->lengths[LOGIN_INDEX_ADDR] = addr_len;
    login->lengths[LOGIN_INDEX_PORT] = port_len;
    login->lengths[LOGIN_INDEX_NAME] = 0;
}

