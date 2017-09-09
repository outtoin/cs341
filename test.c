#include <stdio.h>
#include <ctype.h>
#include <string.h>

char *encrypt(char *dest, const char *src, const char *key);
void main()
{
    char    source[100] = "welcome!";
    char    encry[100];

    printf("%d\n", *encry);
    encrypt(encry, source, "hello");

}

char *encrypt(char *res, const char *src, const char *key)
{
    char *;

    for (origin = res; *src; dest++, src++) {
        printf("origin : %s\n", origin);
        printf("dest : %s\n", dest);
        printf("src : %s\n", src);
    }
}
