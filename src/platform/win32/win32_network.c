struct Platform_Connection* platform_connect_to_server(const char *address, u16 port)
{
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        int error = WSAGetLastError();
        printf("socket() failed, error = %d\n", error);
        return 0;
    }
    printf("socket() success\n");

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(address);

    int connected = connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (connected != 0)
    {
        int error = WSAGetLastError();
        printf("connect() failed, error = %d\n", error);
        closesocket(sock);
        return 0;
    }
    printf("connect(...) success\n");

    u_long mode = 1;
    int result = ioctlsocket(sock, FIONBIO, &mode);
    if (result != NO_ERROR)
    {
        int error = WSAGetLastError();
        printf("ioctlsocket failed, error = %d\n", error);
        closesocket(sock);
        return 0;
    }
    printf("fcntl() success\n");

    struct Platform_Connection *connection = malloc(sizeof(struct Platform_Connection));
    if (!connection)
    {
        printf("malloc() failed\n");
        closesocket(sock);
        return 0;
    }
    printf("connection established\n");

    connection->socket_fd = sock;
    return connection;
}

void platform_disconnect_from_server(struct Platform_Connection *connection)
{
    closesocket(connection->socket_fd);
}

bool platform_send(struct Platform_Connection *connection, void *buffer, u64 size)
{
    printf("sending...\n");
    int sent = send(connection->socket_fd, buffer, size, 0);
    printf("sent %d/%llu bytes\n", sent, size);
    return sent >= 0;
}

s32 platform_receive(struct Platform_Connection *connection, void *buffer, u64 size)
{
    printf("receving...\n");
    int recvd = recv(connection->socket_fd, buffer, size, 0);
    printf("recvd %d bytes\n", recvd);
    if (recvd == -1)
    {
        return 0;
    }
    return recvd;
}

bool platform_init_networking()
{
    static WSADATA wsaData;// = {0};
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        wprintf(L"WSAStartup failed: %d\n", iResult);
        return false;
    }
    printf("WSAStartup done\n");
    return true;
}
