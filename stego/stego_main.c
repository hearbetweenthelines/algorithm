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
#include "../utils/wavtool.h"
#include "stego_main.h"

typedef int bool;
#define true 1
#define false 0

bool isEncode = true;
char *msgFilename;
char *audioFilename;
char *outputFilename;
int pin = -1;

int parseCmd(int argc, char *argv[])
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

int encodeCycle() {}

int main(int argc, char *argv[])
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
    return 0;
}