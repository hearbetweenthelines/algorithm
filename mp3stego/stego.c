#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "compress.h"
#include "crypto.h"
#include "stego.h"

short len = 0;
char *buffer;

// Open the file to be stegoed and load it into memory
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

// Perform the encryption and compression
int compress_encrpty(char *pin)
{
    int temp = len;
    len = m_compress(&buffer, len);
    if (buffer == NULL)
        return 0;
    printf("Compression finished. (ratio: %.1f%%)\n", (double)len * 100 / temp);
    
    len = m_encrypt(&buffer, len, pin, strlen(pin));
    if (buffer == NULL)
        return 0;
    printf("Encryption finished.\n");
    
    char *nBuffer = (char *)malloc(len + 2);
    memcpy(nBuffer, &len, 2);
    memcpy(nBuffer + 2, buffer, len);
    free(buffer);
    buffer = nBuffer;
    len += 2;
    return len;
}

// Return the next bit of the message (after call compress_encrpty)
int getBit(int current)
{
    if (current / 8 < len)
        return (buffer[current / 8] >> (7 - current % 8)) & 0x1;
    else
        return 0;
}

// Perform stego in a way that does not corrupt mp3 structure
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
    	// Search for the correct place to embed next bit
        if (tempbuf[i] == 0xFF && tempbuf[i + 1] == 0xFB)
        {
            if (((tempbuf[i + 2] & 0xF0) >> 4) < 15 && ((tempbuf[i + 2] & 12) >> 2) < 4 &&
                tempbuf[i + 4] == 0)
            {
            	tempbuf[i + 2] = (tempbuf[i + 2] & 0xFE) | getBit(count++);
            }
        }
    }
    if (count < len)
    {
    	printf("Message is not embeded in completely.\n");
    	return -1;
    }

    file = fopen(filename, "wb");
    fwrite(tempbuf, 1, size, file);
    fclose(file);

    return count;
}
