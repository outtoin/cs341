#include <stdio.h>
#include <stdlib.h>

//#define PORT 4000
//#define BUF_LEN 1024

int main(int argc, char *argv[])
{
  	char  	*ipaddr;
  	int   	port;

  	for (int i = 1; i < argc; i++)
  	{
		if (strcmp(argv[i], "-h"))
		{
	  		if(i + 1 <= argc - 1)
	  		{
		  		i++;
				ipaddr = argv[i];
				printf("ipaddr is : %s", ipaddr);
	  		}
		}

		else if (strcmp(argv[i], "-p"))
		{
			if (i + 1 <= argc - 1)
			{
				i++;
				port = atoi(argv[i]);
				printf("port is : %d", port);
			}
		}
  	}

  	return 0;
}
