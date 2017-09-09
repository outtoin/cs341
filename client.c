#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define BUF_LEN 1024

void main(int argc, char *argv[])
{
	// for handling command-line parameter
	char  	*ipaddr;
	int   	port;
	int		operation;
	char	*keyword;

	// for handling socket parameter
	int		c_socket;
	struct 	sockaddr_in c_addr;
	int		c_len;
	char	res[BUF_LEN];
	int		n;

	// handling command-line parameter
  	for (int i = 1; i < argc; i++)
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
				operation = atoi(argv[i]);
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

	printf("[Create Socket]: Success\n");

	// server settings
	c_addr.sin_family = AF_INET;
	c_addr.sin_addr.s_addr = inet_addr(ipaddr);
	c_addr.sin_port = htons(port);

	c_len = sizeof(c_addr);

	// connect to server
	if (connect(c_socket, (struct sockaddr *)&c_addr, c_len))
	{
		printf("[Connect Server]: Failed\n");
		close(c_socket);
		exit(0);
	}

	printf("[Connect Server]: Success\n");

	close(c_socket);
}
