#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/wait.h>
#include <endian.h>

#define BUF_LEN 10000000
#define TOTALC 10

void encrypt(unsigned char *encryptee, unsigned char *buffer, const char *key, int *keyloc);
uint16_t checksum(const char *buf, unsigned size);
ssize_t rio_readn(int fd, void *usrbuf, size_t n);
ssize_t rio_writen(int fd, void *usrbuf, size_t n);

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

    int pid, client_socket;
    static int counter = 0;

    int str_len3, str_len2, str_len1;


    while(1)
    {
        //fprintf(stderr, "in outer while loop\n");
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_size);

        if(client_socket == -1)
        {
            fprintf(stderr, "failed accept\n");
            exit(1);
        }

        if((pid = fork()) == -1)
        {
            fprintf(stderr, "fork error");
            close(client_socket);
            exit(1);
        }

        else if (pid > 0)
        {

        }

        else if (pid == 0)
        {
            counter++;
            //fprintf(stderr, "counter : %d\n", counter);

            int op;
            char keyword[4];
            int length1, length;
            int loc = 0;

            while(1)
            {
                str_len3 = rio_readn(client_socket, message, 16);
                //fprintf(stderr, "str_len3 : %d\n", str_len3);

                memcpy(&op, message, 2);

                op = ntohs(op);

                memcpy(keyword, message+4, 4);
                memcpy(&length, message+8, 8);

                //length = be64toh(length1);

                /*int j;
        		for(j = 0; j < 19; j++) {
        			printf("%02x\t\n", (unsigned char)message[j]);
        		}*/

                if (str_len3 < 8)
                {
                    close(client_socket);
                    break;
                }

                str_len2 = rio_readn(client_socket, message, length-16);
                //fprintf(stderr, "str_len2 : %d\n", str_len2);

                encrypt(str_buffer, message+16, keyword, &loc);

                memset(message+2, 0, 2);
                memset(message+8, 0, strlen(str_buffer));
                memcpy(message+8, str_buffer, strlen(str_buffer));

                uint16_t check_sum1 = (uint16_t)checksum(message, length);
                memcpy(message+2, &check_sum1, 2);

                memset(message+8, 0, strlen(str_buffer));
                rio_writen(client_socket, message, length);

                memset(message, 0, BUF_LEN);
                memset(str_buffer, 0, BUF_LEN-16);
                memset(write_message, 0, BUF_LEN);
            }
        }
    }

    str_len3 = rio_readn(client_socket, message, 8);
    fprintf(stderr, "str_len3 : %d\n", str_len3);


    close(client_socket);
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

unsigned short checksum(const char *buf, unsigned size)
{
	unsigned sum = 0;
	int i;

	/* Accumulate checksum */
	for (i = 0; i < size - 1; i += 2)
	{
		unsigned short word16 = *(unsigned short *) &buf[i];
		sum += word16;
	}

	/* Handle odd-sized case */
	if (size & 1)
	{
		unsigned short word16 = (unsigned char) buf[i];
		sum += word16;
	}

	/* Fold to get the ones-complement result */
	while (sum >> 16) sum = (sum & 0xFFFF)+(sum >> 16);

	/* Invert to get the negative in ones-complement arithmetic */
	return ~sum;
}

void encrypt(unsigned char *encryptee, unsigned char *buffer, const char *key, int *keyloc) {
    char    *res;
    int     len = strlen(key);
    int     i;
    uint64_t length;

    memcpy(&length, buffer+8, 8);

    //for (res = encryptee; *buffer; encryptee++, buffer++)
    for (i=16; i < length; i++)
    {
        unsigned char buffer_i = buffer[i];
        // if alphabet
        if (isalpha(buffer_i))
        {
            buffer_i = (buffer_i - 'a' + (key[*keyloc] - 'a')) % 26 + 'a';
            memcpy(buffer+1, &buffer_i, 1);
            *keyloc = (*keyloc + 1) % len;
        }

        else
        {
            *encryptee = *buffer;
        }
    }

    for (i=16; i < length; i++)
    {
        unsigned char buffer_i = buffer[i];
        if (isalpha(buffer_i))
        {
            encryptee[i] = buffer_i;
        }
    }
}
