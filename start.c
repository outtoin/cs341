#include <stdio.h>
#include <cs50.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUF_LEN 1024

void main(int argc, char *argv[])
{
    // handling file stdin
    char    buf[BUF_LEN];
    size_t  nread;

    // for vigenere cipher parameter
    int shift;
    int KeyValue;

    while((nread = fgets(buf, BUF_LEN, stdin)) > 0)
    {
        int     ix;

        for (ix = 0; ix < strlen(buf); ix++){
            buf[ix] = (char)tolower(buf[ix]);
        }

        printf("%s\n", buf);
    }
}
