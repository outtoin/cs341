#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

//#define PORT 4000
//#define BUF_LEN 1024

int main(int argc, char *argv[])
{
  //int s, n;
  //char *haddr;
  //struct sockaddr_in server_addr;
  //char buf[BUF_LEN+1];

  char  ipaddr;
  int   port = 0;
  int   operation = 0;
  char  keyword;

  if(argc == 1) {
    fputs("error!\n", stderr);
    exit(1);
  }

  for (int i = 1; i < argc; i++) {
      if (strcmp(argv[i], "-h") == 0)
      {
        if (i + 1 <= argc - 1)
        {
          i++;
          ipaddr = argv[i];
          //printf('ip address : %s', ipaddr);
        }
      }

      else if (strcmp(argv[i], "-p") == 0)
      {
        if (i + 1 <= argc - 1)
        {
          i++;
          port = atoi(argv[i]);
          //printf('port : %d', port);
        }
      }

      else if (strcmp(argv[i], "-o") == 0)
      {
        if (i + 1 <= argc - 1)
        {
          i++;
          operation = atoi(argv[i]);
          //printf('operation : %d', operation);
        }
      }

      else if (strcmp(argv[i], "-k") == 0)
      {
        if (i + 1 <= argc - 1)
        {
          i++;
          keyword = argv[i];
          //printf('keyword : %s', keyword);
        }
      }
  }

  return 0;
}
