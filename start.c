#include "stdio.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "netinet/in.h"

//#define PORT 4000
//#define BUF_LEN 1024

void main(int argc, char *argv[])
{
  //int s, n;
  //char *haddr;
  //struct sockaddr_in server_addr;
  //char buf[BUF_LEN+1];

  if(argc == 1) {
    fputs("error!\n", stderr);
    exit(1);
  }

  for (int i = 1; i < argc; i++) {
    print("argc[%d] = %s\n", i, argv[i]);
  }

  return 0;
}
