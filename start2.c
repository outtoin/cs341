#include <stdio.h>
#include <stdlib.h>

//#define PORT 4000
//#define BUF_LEN 1024

int main(int argc, char *argv[])
{
  	char  	*ipaddr;
	int   	port;
	int		operation;
	char	*keyword;

  	for (int i = 1; i < argc; i++)
  	{
		if (strcmp(argv[i], "-h") == 0)
		{
	  		if(i + 1 <= argc - 1)
	  		{
		  		i++;
				ipaddr = argv[i];
				printf("ipaddr is : %s\n", ipaddr);
	  		}
		}

		else if (strcmp(argv[i], "-p") == 0)
		{
			if (i + 1 <= argc - 1)
			{
				i++;
				port = atoi(argv[i]);
				printf("port is : %d\n", port);
			}
		}

		else if (strcmp(argv[i], "-o") == 0)
		{
			if (i + 1 <= argc - 1)
			{
				i++;
				operation = atoi(argv[i]);
				printf("port is : %d\n", operation);
			}
		}

		else if (strcmp(argv[i], "-k") == 0)
		{
			if (i + 1 <= argc - 1)
			{
				i++;
				keyword = argv[i];
				printf("port is : %d\n", keyword);
			}
		}
  	}

  	return 0;
}
