struct Platform_Connection* platform_connect_to_server(const char *address, u16 port)
{
	struct sockaddr_in server;
	
    // create
	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1)
    {
        printf("socket(...) failed\n");
        return 0;
	}
	
	server.sin_addr.s_addr = inet_addr(address);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

    // connect
    int connected = connect(socket_fd, (struct sockaddr *)&server, sizeof(server));
    if (connected == -1)
    {
		printf("connect(...) failed\n");
        close(socket_fd);
        return 0;
    }

#if 1
    // make scoket non-blocking
    int set_non_blocking = fcntl(socket_fd, F_SETFL, fcntl(socket_fd, F_GETFL, 0) | O_NONBLOCK);
    if (set_non_blocking == -1)
    {
        printf("error: fcntl failed to set socket non-blocking\n");
        close(socket_fd);
        return 0;
    }
#endif

    struct Platform_Connection *connection = (struct Platform_Connection*)malloc(sizeof(struct Platform_Connection));
    connection->socket_fd = socket_fd;
    return connection;
}

void platform_disconnect_from_server(struct Platform_Connection *connection)
{
    close(connection->socket_fd);
}

bool platform_send(struct Platform_Connection *connection, void *buffer, u64 size)
{
    int sent = send(connection->socket_fd, buffer, size, 0);
    return sent >= 0;
}

s32 platform_receive(struct Platform_Connection *connection, void *buffer, u64 size)
{
    ssize_t recvd = recv(connection->socket_fd, buffer, size, 0);
    if (recvd == -1)
    {
        return 0;
    }
    return recvd;
}

