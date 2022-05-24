struct Platform_Connection* platform_connect_to_server(const char *address, u16 port)
{
    return 0;
}

void platform_disconnect_from_server(struct Platform_Connection *connection)
{
}

bool platform_send(struct Platform_Connection *connection, void *buffer, u64 size)
{
    return false;
}

s32 platform_receive(struct Platform_Connection *connection, void *buffer, u64 size)
{
    return 0;
}
