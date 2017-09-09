#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUF_LEN 1024

char *encrypt(char *, const char *buffer, const char *key, int *keyloc);
void main(int argc, char *argv[])
{
    // handling file stdin
    char    buf[BUF_LEN];
    size_t  nread;

    // for vigenere cipher parameter
    char    encbuf[BUF_LEN];
    int     loc = 0;

    while((nread = fgets(buf, BUF_LEN, stdin)) > 0)
    {
        for (int ix = 0; ix < strlen(buf); ix++){
            buf[ix] = (char)tolower(buf[ix]);
        }

        encrypt(encbuf, buf, "abcd", &loc);
        printf("%s", encbuf);
    }
}

char *encrypt(char *encryptee, const char *buffer, const char *key, int *keyloc) {
    char    *res;
    int     len = strlen(key);

    for (res = encryptee; *buffer; encryptee++, buffer++)
    {
        // if alphabet
        if (isalpha(*buffer))
        {
            *encryptee = (*buffer - 'a' + (key[*keyloc] - 'a')) % 26 + 'a';
            *keyloc = (*keyloc + 1) % len;
        }

        else
        {
            *encryptee = *buffer;
        }
    }
    *encryptee = '\0';
    return res;
}
