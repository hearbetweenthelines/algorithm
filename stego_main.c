/*
-parseCmd
-openMsg
-compressMsg
-encryptMsg
-openAudio
-stego
-output
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "utils/wavtool.h"
#include "compress/compress.h"
#include "crypto/crypto.h"
#include "stego/stego.h"
#include "stego_main.h"

typedef int bool;
#define true 1
#define false 0

bool isEncode = true;
char *msgFilename;
char *audioFilename;
char *outputFilename;
char const* header = "HBL";
int pin = -1;

int parseCmd(int argc, char const *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-d") == 0)
        {
            isEncode = false;
            pin      = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-e") == 0)
        {
            isEncode = true;
            pin      = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-m") == 0)
            msgFilename = argv[++i];
        else if (strcmp(argv[i], "-a") == 0)
            audioFilename = argv[++i];
        else if (strcmp(argv[i], "-o") == 0)
            outputFilename = argv[++i];
        else
            return UNDEFINED_CMD;
    }
    if (isEncode)
    {
        if (msgFilename == NULL || audioFilename == NULL || outputFilename == NULL || pin == -1)
            return WRONG_FORMAT_ENCODE;
    }
    else
    {
        if (audioFilename == NULL || outputFilename == NULL || pin == -1)
            return WRONG_FORMAT_DECODE;
    }
    return OK;
}

void printEncodeUsage() { printf("Usage: hbl -e pin -m FileToHide -a UsedAudio -o OutputFile\n"); }
void printDecodeUsage() { printf("Usage: hbl -d pin -a UsedAudio -o OutputFile\n"); }
void printUsage()
{
    printEncodeUsage();
    printDecodeUsage();
}

int getFileSize(char const *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
        return -1;
    fseek(file, 0, SEEK_END);
    int size = (int)ftell(file);
    fclose(file);
    return size;
}

int openMsgFile(char const *filename, int length, const char *buffer, int offset)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
        return -1;

    fread(buffer + offset, sizeof(char), length, file);
    fclose(file);
    return 0;
}

double **openAudioFile(char const *filename, WAVE_INFO *wave_info)
{
    int r = open_wave(filename, wave_info);
    if (r == FILE_OPEN_ERROR || r == WAVE_NOT_MATCH || r == FMT_NOT_MATCH || r == DATA_NOT_FOUND)
        return NULL;
    return wave_read(wave_info, 0);
}

int encodeCycle()
{
    int size  = getFileSize(msgFilename);
    int filenameLength = strlen(msgFilename);
    char* msg;
    if (size != -1)
    {
        msg = (char *)malloc(size + filenameLength + sizeof(int) * 2 + 3);
        if (openMsgFile(msgFilename, size, msg, filenameLength + sizeof(int) * 2 + 3) == -1)
            return MSG_OPEN_FAIL;
        memcpy(msg, header, 3);
        memcpy(msg + 3, &filenameLength, sizeof(int));
        memcpy(msg + 3 + sizeof(int), msgFilename, filenameLength);
        memcpy(msg + 3 + sizeof(int) + filenameLength, &size, sizeof(int));
    }
    else
    {
        msg = (char*)malloc(strlen(msgFilename) + sizeof(int) * 2 + 3);
        memcpy(msg, header, 3);
        memcpy(msg + 3, &filenameLength, sizeof(int));
        memcpy(msg + 3 + sizeof(int), msgFilename, filenameLength);
        int zero = 0;
        memcpy(msg + 3 + sizeof(int) + filenameLength, &zero, sizeof(int));
    }

    /*int temp1 = 0; 
    memcpy(&temp1,msg, sizeof(int));
    printf("%d\n", temp1);
    for (int i = 0; i < temp1; i++)
        printf("%c", msg[i + sizeof(int)]);
    printf("\n");
    int temp2 = 0;
    memcpy(&temp2, msg + sizeof(int) + temp1, sizeof(int));
    printf("%d\n", temp2);
    for (int i = 0; i < temp2; i++)
    {
        printf("%c", msg[i + 2 * sizeof(int) + temp1]);
    }
    printf("\n");*/

    //return 0;


    WAVE_INFO wave_info;

    double **audio = openAudioFile(audioFilename, &wave_info);

    msg = compress(msg);
    if (msg == NULL)
        return COMPRESS_FAIL;
    msg = crypto(msg);
    if (msg == NULL)
        return ENCRYPTION_FAIL;

    stego(msg, size, audio, &wave_info, outputFilename);

    free(msg);
    // free(audio);
}
int decodeCycle()
{
    char* msgFlow = destego(audioFilename);
}

int main(int argc, char const *argv[])
{
    switch (parseCmd(argc, argv))
    {
    case OK:
        break;
    case UNDEFINED_CMD:
        printUsage();
        return -1;
    case WRONG_FORMAT_ENCODE:
        printEncodeUsage();
        return -1;
    case WRONG_FORMAT_DECODE:
        printDecodeUsage();
        return -1;
    }
    if (isEncode)
        encodeCycle();
    else
        decodeCycle();
    return 0;
}
