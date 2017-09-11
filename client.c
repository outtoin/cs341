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
uint16_t checksum2(const char *buf, unsigned size);
ssize_t rio_readn(int fd, void *usrbuf, size_t n);
ssize_t rio_writen(int fd, void *usrbuf, size_t n);
void main(int argc, char *argv[])
{
	// for handling command-line parameter
	char  		*ipaddr;
	int   		port;
	uint16_t	operation;
	char		*keyword;
	int			i;

	// for messages
	unsigned char 	*message;
	unsigned char 	*read_message;
	unsigned char 	*str_buffer;
	int 			str_len1, str_len2;
	unsigned short	check_sum;
	uint64_t		length;

	// for handling socket parameter
	int		c_socket;
	struct 	sockaddr_in c_addr;
	int		c_len;

	message = (unsigned char *) malloc(sizeof(unsigned char) * BUF_LEN);
	memset(message, 0, BUF_LEN);
	read_message = (unsigned char *) malloc(sizeof(unsigned char) * BUF_LEN);
	memset(read_message, 0, BUF_LEN);
	str_buffer = (unsigned char *) malloc(sizeof(unsigned char) * BUF_LEN-16);
	memset(str_buffer, 0, BUF_LEN-16);


	// handling command-line parameter
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

	// make socket & validation
	if ((c_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("[Create Socket]: Failed\n");
		exit(0);
	}

	// server settings
	c_addr.sin_family = AF_INET;
	c_addr.sin_addr.s_addr = inet_addr(ipaddr);
	c_addr.sin_port = htons(port);

	c_len = sizeof(c_addr);

	// connect to server
	if (connect(c_socket, (struct sockaddr *)&c_addr, c_len) == -1)
	{
		printf("[Connect Server]: Failed\n");
		close(c_socket);
		exit(0);
	}

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

		uint16_t checksum_test = htons('\x4d\x2e');
		printf("%x\n", checksum_test);
		//uint16_t check_sum = checksum2(message, be64toh(length));
		printf("check_sum : %x\n", check_sum1);
		memcpy(message+2, &check_sum, sizeof(char) * 2);

		int j;
		for(j = 0; j < leeen; j++) {
			printf("%02x\t", (unsigned char)message[j]);
		}

		printf("\nread_message: \n");

		rio_writen(c_socket, message, be64toh(length));

		str_len2 = rio_readn(c_socket, read_message, be64toh(length));

		int k;
		for(k = 0; k < be64toh(length); k++) {
			printf("%02x\t", (unsigned char)read_message[j]);
		}
	}



	close(c_socket);
}

// TCP checksum refer : http://locklessinc.com/articles/tcp_checksum/

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

uint16_t checksum2(const char *buf, unsigned size)
{
	uint64_t sum = 0;
	const uint64_t *b = (uint64_t *) buf;

	unsigned t1, t2;
	uint16_t t3, t4;

	/* Main loop - 8 bytes at a time */
	while (size >= sizeof(uint64_t))
	{
		uint64_t s = *b++;
		sum += s;
		if (sum < s) sum++;
		size -= 8;
	}

	/* Handle tail less than 8-bytes long */
	buf = (const char *) b;
	if (size & 4)
	{
		unsigned s = *(unsigned *)buf;
		sum += s;
		if (sum < s) sum++;
		buf += 4;
	}

	if (size & 2)
	{
		uint16_t s = *(uint16_t *) buf;
		sum += s;
		if (sum < s) sum++;
		buf += 2;
	}

	if (size)
	{
		unsigned char s = *(unsigned char *) buf;
		sum += s;
		if (sum < s) sum++;
	}

	/* Fold down to 16 bits */
	t1 = sum;
	t2 = sum >> 32;
	t1 += t2;
	if (t1 < t2) t1++;
	t3 = t1;
	t4 = t1 >> 16;
	t3 += t4;
	if (t3 < t4) t3++;

	return ~t3;
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
