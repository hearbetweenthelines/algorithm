#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "compress.h"
#include "crypto.h"
#include "stego.h"

short len = 0;
char *buffer;


int openFile(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
        return 0;
    fseek(file, 0, SEEK_END);
    len = (short)ftell(file);

    buffer = (char *)malloc(len);
    fseek(file, 0, SEEK_SET);
    fread(buffer, sizeof(char), (int)len, file);
    return len;
}

int compress_encrpty(char *pin)
{
    //printf("%d\n", len);
    int temp = len;
    len = m_compress(&buffer, len);
    if (buffer == NULL)
        return 0;
    printf("Compression finished. (ratio: %.1f%%)\n", (double)len * 100 / temp);
    //printf("%d\n", len);
    //for (int i = 0; i < len; i++)
    //    printf("%02hhX ", buffer[i]);
    //printf("\n");
    len = m_encrypt(&buffer, len, pin, strlen(pin));
    if (buffer == NULL)
        return 0;
    printf("Encryption finished.\n");
    // for (int i = 0; i < len; i++)
    //     printf("%02hhX ", buffer[i]);
    // printf("\n");
    char *nBuffer = (char *)malloc(len + 2);
    memcpy(nBuffer, &len, 2);
    memcpy(nBuffer + 2, buffer, len);
    free(buffer);
    buffer = nBuffer;
    len += 2;
    //for (int i = 0; i < len; i++)
    //    printf("%02hhX ", buffer[i]);
    //printf("\n");
    return 0;
    return len;
}

int getBit(int current)
{
    if (current / 8 < len)
    {
        return (buffer[current / 8] >> (7 - current % 8)) & 0x1;
    }
    else
    {
        return 0;
    }
}

int performStego(char* filename)
{
	FILE *file = fopen(filename, "r");
    if (file == NULL)
        return 0;
    fseek(file, 0, SEEK_END);
    int size = (int)ftell(file);

    unsigned char* tempbuf = (unsigned char *)malloc(size);
    fseek(file, 0, SEEK_SET);
    fread(tempbuf, sizeof(char), size, file);
    fclose(file);
    int count = 0;
    int i;
    for (i = 0; i < size && count != len * 8; i += 2)
    {
        if (tempbuf[i] == 0xFF && tempbuf[i + 1] == 0xFB)
        {
            if (((tempbuf[i + 2] & 0xF0) >> 4) < 15 && ((tempbuf[i + 2] & 12) >> 2) < 4 &&
                tempbuf[i + 4] == 0)
            {
            	tempbuf[i + 2] = (tempbuf[i + 2] & 0xFE) | getBit(count++);
            	//if (count < 16)
            		//printf("%d %02hhX%02hhX %02hhX%02hhX %02hhX%02hhX\n", tempbuf[i + 2] & 1,
                    //   tempbuf[i], tempbuf[i + 1], tempbuf[i + 2], tempbuf[i + 3], tempbuf[i + 4],
                    //   tempbuf[i + 5]);
            }
        }
    }
    if (count < len)
    {
    	printf("Message not in completely.\n");
    	return -1;
    }

	file = fopen(filename, "wb");
	fwrite(tempbuf, 1, size, file);
	fclose(file);

    return count;
}

/*int main(int argc, char const *argv[])
{
    openFile(argv[1]);
    for (int i = 0; i < len; i++)
        printf("%02hhX ", buffer[i]);
    printf("\n");
    compress_encrpty();

    short l = 0;
    for (int i = 8; i < 16; i++)
    {
        printf("%d\n", getBit(i));
        // printf("%d\n", getBit(i));
        l = l << 1 | getBit(i);
    }
    for (int i = 0; i < 8; i++)
    {
        printf("%d\n", getBit(i));
        // printf("%d\n", getBit(i));
        l = l << 1 | getBit(i);
    }
    printf("%d\n", l);
    return 0;
}*/