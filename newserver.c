#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/wait.h>

#define BUF_LEN 10000000
#define TOTALC 10

void main(int argc, char *argv[])
{
    unsigned char 	*message;
    unsigned char 	*write_message;
    unsigned char 	*str_buffer;

    message = (unsigned char *) malloc(sizeof(unsigned char) * BUF_LEN);
    memset(message, 0, BUF_LEN);
    write_message = (unsigned char *) malloc(sizeof(unsigned char) * BUF_LEN);
    memset(write_message, 0, BUF_LEN);
    str_buffer = (unsigned char *) malloc(sizeof(unsigned char) * (BUF_LEN-16));
    memset(str_buffer, 0, BUF_LEN-16);

    int i, port;
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-p") == 0)
        {
            if(i + 1 <= argc - 1)
            {
                i++;
                port = atoi(argv[i]);
            }
        }
    }

    int server_socket;
    server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        fprintf(stderr, "failed to make server socket\n");
        exit(1);
    }

    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    printf("%d\n", port);

    if(bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        fprintf(stderr, "bind error\n");
        exit(1);
    }

    if (listen(server_socket, 5) == -1)
    {
        fprintf(stderr, "listen failed\n");
        exit(1);
    }

    struct  sockaddr_in client_addr;
    int     client_addr_size;
    client_addr_size = sizeof(client_addr);

    int client_socket;
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_size);

    if(client_socket == -1)
    {
        fprintf(stderr, "failed accept\n");
        exit(1);
    }

    int str_len3, str_len2, str_len1;

    str_len3 = rio_readn(client_socket, message, 8);
    fprintf(stderr, "str_len3 : %d\n", str_len3);


    close(client_socket);
}
