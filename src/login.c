static void login_process_network_event(struct State *state, struct Login *login, u8 *buff, u32 buff_len)
{
    if (!(login->flags & LOGIN_WAITING))
    {
        printf("invalid network event in login");
        return;
    }

    u16 *type = (u16*)buff;
    printf("buff_len = %d\n", buff_len);
    if (buff_len < sizeof(u16))
    {
        printf("message too small\n");
        return;
    }

    if (*type == FCP_S_STATUS)
    {
        struct FCP_S_Status *status = (struct FCP_S_Status*)buff;
        if (buff_len < sizeof(struct FCP_S_Status))
        {
            printf("message too small for status\n");
            return;
        }

        if (status->status == FCP_S_SUCCESS)
        {
            printf("changing state from login to chat\n");
            login->flags ^= LOGIN_WAITING;
            login->flags |= LOGIN_SUCCESS;
            state_change(state, STATE_CHAT);
        }
        else
        {
            // TODO: proper behaviour
            printf("login unsuccessful\n");
        }
    }
    else
    {
        printf("invalid serer message in login\n");
        return;
    }
}

static void login_process_window_event(struct Login *login, struct Window_Event *event)
{
    enum Window_Event_Type type = event->type;

    if (type == WINDOW_KEY)
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
                u16 port_u16 = atoi(login->values[LOGIN_INDEX_PORT]);
                const char *addr = login->values[LOGIN_INDEX_ADDR];

                struct Platform_Connection *connection = platform_connect_to_server(addr, port_u16);
                if (!connection)
                {
                    login->flags |= LOGIN_FAILED;
                    return;
                }
                login->connection = connection;
                
                struct FCP_C_Login_Desc desc;
                desc.type = FCP_C_LOGIN;
                desc.name_len = login->lengths[LOGIN_INDEX_NAME];

                u32 package_size = sizeof(desc) + desc.name_len;
                u8 package[package_size];
                memcpy(package, &desc, sizeof(desc));
                memcpy(package + sizeof(desc), login->values[LOGIN_INDEX_NAME], desc.name_len);
                platform_send(login->connection, package, package_size);
                printf("sent %d bytes\n", package_size);

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
                printf("not allowing space here\n");
            }
            else if (ch == 8)
            {
                u16 val_len = login->lengths[login->index];
                if (val_len > 0)
                {
                    u16 *length = &login->lengths[login->index];
                    char *value = login->values[login->index];

                    *length -= 1;
                    value[*length] = '\0';
                }
            }
            else
            {
                char *value  = login->values[login->index];
                u16  *value_len = &(login->lengths[login->index]);

                if (*value_len != 127)
                {
                    u32 it = *value_len;
                    value[it] = ch;
                    value[it+1] = '\0';
                    *value_len += 1;
                }
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

    memset(login->values[LOGIN_INDEX_NAME], 0, 1);
    memcpy(login->values[LOGIN_INDEX_ADDR], addr, addr_len + 1);
    memcpy(login->values[LOGIN_INDEX_PORT], port, port_len + 1);

    login->lengths[LOGIN_INDEX_ADDR] = addr_len;
    login->lengths[LOGIN_INDEX_PORT] = port_len;
    login->lengths[LOGIN_INDEX_NAME] = 0;
}

