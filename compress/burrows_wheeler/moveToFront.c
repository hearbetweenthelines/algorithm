#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *encode(char *data, int len)
{
    char *chars = (char *)malloc(256);
    for (int i = 0; i < 256; i++)
        chars[i] = (char)i;
    char *result = (char *)malloc(len);
    for (int i = 0; i < len; i++)
    {
        char ch = data[i];
        char tmpin, count, tmpout;
        for (count = 0, tmpout = chars[0]; ch != chars[count]; count++)
        {
            tmpin        = chars[count];
            chars[count] = tmpout;
            tmpout       = tmpin;
        }
        chars[count] = tmpout;
        chars[0]     = ch;
        result[i]    = count;
    }

    return result;
}

char *decode(char *data, int len)
{
    char *chars = (char *)malloc(256);
    for (int i = 0; i < 256; i++)
        chars[i] = (char)i;
    char *result = (char *)malloc(len);

    for (int i = 0; i < len; i++)
    {
        char count = data[i];
        result[i]  = chars[count];
        char index = chars[count];
        while (count > 0)
        {
            chars[count] = chars[--count];
        }
        chars[0] = index;
    }

    return result;
}

int main(int argc, char const *argv[])
{
    char *a = "CAAABCCCACCF";
    char *r = encode(a, strlen(a));
    for (int i = 0; i < strlen(a); i++)
        printf("%02hhX ", r[i]);
    printf("\n");
    char *d = decode(r, strlen(a));
    for (int i = 0; i < strlen(a); i++)
        printf("%c ", d[i]);
    printf("\n");
    return 0;
}