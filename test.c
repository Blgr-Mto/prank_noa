#define _GNU_SOURCE

#include <arpa/inet.h>
#include <fnmatch.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

static void communicate(int client_fd)
{
    ssize_t bytes = 0;
    char buffer[BUFFER_SIZE];
    while ((bytes = recv(client_fd, buffer, BUFFER_SIZE, 0)) > 0)
    {
        printf("Test\n");
        // close(client_fd);
    }
}

int create_and_bind()
{
    struct addrinfo hints = { 0 };
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    struct addrinfo *res = NULL;

    if (getaddrinfo("localhost", "8080", &hints, &res) == -1)
    {
        fprintf(stderr, "create_and_bind: failed getaddrinfo\n");
        return -1;
    }

    int sock = -1;

    for (struct addrinfo *p = res; p; p = p->ai_next)
    {
        sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sock == -1)
            continue;

        int val = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
        if (bind(sock, res->ai_addr, res->ai_addrlen) != -1)
            break;

        close(sock);
        sock = -1;
    }

    freeaddrinfo(res);

    return sock;
}

void start_server(int server_socket)
{
    if (listen(server_socket, SOMAXCONN) == -1)
        return;

    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_fd = accept(server_socket, &(client_addr), &(addr_len));

        if (client_fd != -1)
        {
            communicate(client_fd);
            close(client_fd);
        }
    }
}

/*
int daemon_start_server(void)
{
    pid_t d_pid = fork();

    if (d_pid == -1)
    {
        fprintf(stderr, "deamon_create: error on fork");
        exit(0);
    }

    if (d_pid == 0)
    {
        int server_socket = create_and_bind();
        if (server_socket == -1)
        {
            fprintf(stderr, "Cannot create socket\n");
            exit(0);
        }
        start_server(logger, server_socket);
        close(server_socket);
    }
    else
    {
        save_daemon(config->pid_file, d_pid);
    }

    return 0;
}
*/

int main(void)
{
    int client_fd = create_and_bind();
    start_server(client_fd);
    // printf("ta grand mere\n");
}
