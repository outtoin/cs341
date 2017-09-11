#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sys/types.h"
#include <endian.h>
#include "netinet/in.h"
#include "sys/socket.h"
#include "arpa/inet.h"
#include <errno.h>

#define BUF_LEN 10000000

unsigned short checksum(const char *buf, unsigned size);
ssize_t rio_readn(int fd, void *usrbuf, size_t n);
ssize_t rio_writen(int fd, void *usrbuf, size_t n);
void main(int argc, char *argv[])
{
    unsigned char 	*message;
    unsigned char 	*read_message;
    unsigned char 	*str_buffer;

    message = (unsigned char *) malloc(sizeof(unsigned char) * BUF_LEN);
    memset(message, 0, BUF_LEN);
    read_message = (unsigned char *) malloc(sizeof(unsigned char) * BUF_LEN);
    memset(read_message, 0, BUF_LEN);
    str_buffer = (unsigned char *) malloc(sizeof(unsigned char) * (BUF_LEN-16));
    memset(str_buffer, 0, BUF_LEN-16);

    int i;
    char *ipaddr;
    int port;
    uint8_t operation;
    char *keyword;
    for (i = 1; i < argc; i++)
  	{
		if (strcmp(argv[i], "-h") == 0)
		{
	  		if(i + 1 <= argc - 1)
	  		{
		  		i++;
				ipaddr = argv[i];
	  		}
		}

		else if (strcmp(argv[i], "-p") == 0)
		{
			if (i + 1 <= argc - 1)
			{
				i++;
				port = atoi(argv[i]);
			}
		}

		else if (strcmp(argv[i], "-o") == 0)
		{
			if (i + 1 <= argc - 1)
			{
				i++;
				operation = (uint8_t) atoi(argv[i]);
			}
		}

		else if (strcmp(argv[i], "-k") == 0)
		{
			if (i + 1 <= argc - 1)
			{
				i++;
				keyword = argv[i];
			}
		}
  	}

    int client_socket;
    client_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        fprintf(stderr, "failed socekt\n");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ipaddr);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        fprintf(stderr, "connect error\n");
        exit(1);
    }

    uint64_t length;
    int 		str_len1, str_len2;

    while(1)
    {
        uint16_t op = htons(operation);
		memcpy(message, &op, sizeof(char) * 2);
		memcpy(message+4, keyword, sizeof(char) * 4);
		if (fread(str_buffer, 1, BUF_LEN-16, stdin) <= 0)
		{
			break;
		}

        printf("strlen : %d\n", strlen(str_buffer));
		uint64_t leeen = (uint64_t)(strlen(str_buffer) + 16);
		length = htobe64(leeen);
		memcpy(message+8, &length, sizeof(char) * 8);
		memcpy(message+16, str_buffer, strlen(str_buffer));
		uint16_t check_sum1 = (uint16_t)checksum(message, be64toh(length));
		uint16_t check_sum = htons(check_sum1);

		//uint16_t checksum_test = htons('\x4d\x2e');
		//printf("%x\n", checksum_test);
		//uint16_t check_sum = checksum2(message, be64toh(length));
		printf("check_sum : %x\n", check_sum1);
		memcpy(message+2, &check_sum, sizeof(char) * 2);

		int j;
		for(j = 0; j < leeen; j++) {
			printf("%02x\t", (unsigned char)message[j]);
		}

		printf("\nread_message: \n");

        str_len2 = rio_writen(client_socket, message, be64toh(length));

        str_len1 = rio_readn(client_socket, read_message, be64toh(length));

        int l;
		for(l = 0; l < be64toh(length); l++) {
			printf("%02x\t", (unsigned char)read_message[l]);
		}

    }
    close(client_socket);
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

ssize_t rio_readn(int fd, void *usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nread;
    char *bufp = usrbuf;

	printf("nleft: %d\n", nleft);

    while (nleft > 0) {
		nread = read(fd, bufp, nleft);
		printf("nread: %d\n", nread);
    	if ((nread < 0)) {
        	if (errno == EINTR) /* Interrupted by sig handler return */
        		nread = 0;      /* and call read() again */
        	else
        		return -1;      /* errno set by read() */
    	}
    	else if (nread == 0)
		{
			break;              /* EOF */
		}

		printf("nread: %d\n", nread);
    	nleft -= nread;
    	bufp += nread;
    }
    return (n - nleft);         /* Return >= 0 */
}

ssize_t rio_writen(int fd, void *usrbuf, size_t n)
{
	size_t 		nleft = n;
	ssize_t 	nwritten;
	char 		*bufp = usrbuf;

	while (nleft > 0)
	{
		nwritten = write(fd, bufp, nleft);
		printf("nwritten: %d\n", nwritten);
		if (nwritten <= 0) {
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
