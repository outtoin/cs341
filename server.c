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


ssize_t rio_readn(int fd, void *usrbuf, size_t n);
ssize_t rio_writen(int fd, void *usrbuf, size_t n);
void main(int argc, char *argv[])
{
    // for handling command-line parameter
    int         port;
    //uint16_t    operation;
    char        *keyword;

    // for messages
    unsigned char 	*message;
    unsigned char 	*write_message;
    unsigned char 	*str_buffer;

    // for handling socket parameter
    int		s_socket;
    int     c_socket[TOTALC];
    struct  sockaddr_in s_addr;
    struct 	sockaddr_in c_addr;
    int		c_len;

    int     str_len1, str_len2, str_len3;

    message = (unsigned char *) malloc(sizeof(unsigned char) * BUF_LEN);
    memset(message, 0, BUF_LEN);
    write_message = (unsigned char *) malloc(sizeof(unsigned char) * BUF_LEN);
    memset(write_message, 0, BUF_LEN);
    str_buffer = (unsigned char *) malloc(sizeof(unsigned char) * (BUF_LEN-8));
    memset(str_buffer, 0, BUF_LEN-8);


    // handling command-line parameter
    int i;
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
    if((s_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "can't create server socket\n");
        exit(1);
    }

    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    s_addr.sin_port = htons(port);
    printf("%d\n", port);

    if(bind(s_socket, (struct sockaddr *)&s_addr, sizeof(s_addr)) < 0)
    {
        fprintf(stderr, "bind() error\n");
        exit(0);
    }

    if (listen(s_socket, 5) == -1) {
        fprintf(stderr, "listen error\n");
        exit(1);
    }

    c_len = sizeof(c_addr);
    int pid, new;
    static int counter = 0;

    /*while(1) {
        fprintf(stderr, "in outer while loop\n");
        new = accept(s_socket, (struct sockaddr *)&c_addr, &c_len);
        if (new == -1)
        {
            fprintf(stderr, "failed connection\n");
            exit(1);
        }
        printf("%d\n", new);
        fprintf(stderr, "in outer while loop\n");
        printf("1\n");
        if((pid = fork()) < -1)
        {
            fprintf(stderr, "fork error");
            close(new);
            exit(1);
        }

        // child
        else if (pid == 0)
        {
            counter++;

            int operation;

            while(1)
            {
                str_len3 = rio_readn(new, message, 8);
                fprintf(stderr, "str_len3: %d\n", str_len3);
                memcpy(&operation, message, 2);
                memcpy(&keyword, message+4, 4);
            }
        }
        else if (pid > 0)
        {
            fprintf(stderr, "pid > 0");
        }
    }
    close(s_socket);*/
}

ssize_t rio_readn(int fd, void *usrbuf, size_t n)
{
	size_t		nleft = n;
	ssize_t 	nread;
	char 		*bufp = usrbuf;

	while (nleft > 0)
	{
		if ((nread = read(fd, bufp, nleft)) < 0)
		{
			if (errno == EINTR)
			{
				nread = 0;
			}
			else
			{
				return -1;
			}
		}

		else if (nread == 0)
		{
			break;
		}

		nleft -= nread;
		bufp += nread;
	}

	return (n - nleft);
}

ssize_t rio_writen(int fd, void *usrbuf, size_t n)
{
	size_t 		nleft = n;
	ssize_t 	nwritten;
	char 		*bufp = usrbuf;

	while (nleft > 0)
	{
		if ((nwritten = write(fd, bufp, nleft)) <= 0) {
			if (errno == EINTR)
			{
				nwritten = 0;
			}
			else{
				return -1;
			}
		}

		nleft -= nwritten;
		bufp += nwritten;
	}

	return n;
}
