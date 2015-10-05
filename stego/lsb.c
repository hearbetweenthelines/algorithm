#include "lsb.h"
#include <math.h>

int utiBit = 8;
int MASK   = 1;

/*
 * Write 'memdata' into 'filename' with format assigned by 'wave_info'
 * Note that the bitDepth is constrained to 24-bit
 */
int simple_write_wave(char const *filename, char *memdata, WAVE_INFO const *wave_info)
{
    FILE *file = fopen(filename, "w");
    if (!file)
        return -1;

    int dataWidth  = wave_info->bitDepth / 8;
    int frameCount = wave_info->dataSize / dataWidth;

    char *buf = (char *)malloc(sizeof(char) * (wave_info->dataSize + 44));
    if (!buf)
    {
        printf("Memory allocation failed.\n");
        fclose(file);
        return -1;
    }
    char *chardata = (char *)malloc(sizeof(char) * 5);
    strcpy(chardata, "RIFF");
    int intdata     = wave_info->dataSize + 36;
    short shortdata = 1;
    if (wave_info->bitDepth == 32)
        shortdata = 3;
    // RIFF WAVE Chunk
    memcpy(buf, chardata, 4);
    memcpy(buf + 4, &intdata, 4);
    strcpy(chardata, "WAVE");
    memcpy(buf + 8, chardata, 4);

    // FMT Chunk
    strcpy(chardata, "fmt ");
    memcpy(buf + 12, chardata, 4);
    intdata = 16;
    memcpy(buf + 16, &intdata, 4);
    memcpy(buf + 20, &shortdata, 2);
    memcpy(buf + 22, &(wave_info->channels), 2);
    memcpy(buf + 24, &(wave_info->sampleRate), 4);
    intdata = wave_info->sampleRate * wave_info->channels * dataWidth;
    memcpy(buf + 28, &intdata, 4);
    shortdata = (unsigned short)(wave_info->channels * dataWidth);
    memcpy(buf + 32, &shortdata, 2);
    memcpy(buf + 34, &(wave_info->bitDepth), 2);

    // Data Chunk
    strcpy(chardata, "data");
    memcpy(buf + 36, chardata, 4);
    memcpy(buf + 40, &(wave_info->dataSize), 4);
    char *start = buf + 44;

    memcpy(start, memdata, frameCount * dataWidth);
    if (fwrite(buf, sizeof(char), wave_info->dataSize, file) != wave_info->dataSize)
    {
        printf("Failed to write file.\n");
        fclose(file);
        return -1;
    }
    fclose(file);
    free(buf);

    return 0;
}

/*
 * Stego function
 * Use the last two bits of each sample(3 bytes) to hide the info
 * Note that each byte in the message needs 4 samples to save(12 bytes)
 */
int lsb_stego(char const *msg, int msglen, double **data, WAVE_INFO *wave_info,
              char const *filename)
{
    for (int i = 0; i < utiBit - 1; i++)
        MASK = (MASK << 1) | 1;
    if (msglen * (8 / utiBit) > wave_info->dataSize)
    {
        printf("Message/file is too long.\n");
        return -1;
    }

    int frameCount      = wave_info->dataSize / (wave_info->bitDepth / 8);
    wave_info->bitDepth = 24;
    int dataWidth       = wave_info->bitDepth / 8;
    double zeroline     = pow(2.0, wave_info->bitDepth - 1) - 1;
    wave_info->dataSize = frameCount * dataWidth;
    // double zeroline = pow(2.0, 24 - 1);

    // Header that contains an indicator and the message length
    char *header = (char *)malloc(3 + sizeof(int));
    header[0]    = 'H';
    header[1]    = 'B';
    header[2]    = 'L';

    memcpy(header + 3, &msglen, sizeof(int));

    char *temp = (char *)malloc(3 + sizeof(int) + msglen);
    memcpy(temp, header, 7);
    memcpy(temp + 7, msg, msglen);
    msglen += 7;
    char *idata = (char *)malloc(frameCount * dataWidth);
    for (int i = 0; i < frameCount; i++)
    {
        int sample = (int)(data[i % wave_info->channels][i / wave_info->channels] * zeroline);
        if (sample > 0xFFFFFF)
            sample = 0xFFFFFF;

        if (msglen * (8 / utiBit) >= i)
        {
            sample &= ~MASK;
            sample |= ((temp[i / (8 / utiBit)] >> ((i % (8 / utiBit)) * utiBit)) & MASK);
            // printf("%d\n", i / (8 / utiBit));
        }
        memcpy(idata + i * dataWidth, &sample, dataWidth);
    }
    if (simple_write_wave(filename, idata, wave_info) == -1)
    {
        free(idata);
        return -1;
    }

    free(idata);
    return 0;
}


char *lsb_destego(char const *audiopath)
{
    for (int i = 0; i < utiBit - 1; i++)
        MASK = (MASK << 1) | 1;
    WAVE_INFO wave_info;
    int r = open_wave(audiopath, &wave_info);
    if (r == FILE_OPEN_ERROR || r == WAVE_NOT_MATCH || r == FMT_NOT_MATCH || r == DATA_NOT_FOUND)
    {
        printf("Failed to open the assigned audio file.\n");
        return NULL;
    }

    FILE *file = fopen(wave_info.filename, "r");
    if (file == NULL)
    {
        printf("Failed to open the assigned audio file.\n");
        return NULL;
    }

    // Load data into memory
    fseek(file, (long)wave_info.data_align, SEEK_SET);
    char *buf = (char *)malloc(wave_info.dataSize);
    fread(buf, sizeof(char), wave_info.dataSize, file);
    fclose(file);

    char *check = (char *)malloc(4);
    memset(check, 0, 4);
    for (int i = 0; i < 3; i++)
        for (int j = (8 / utiBit) - 1; j >= 0; j--)
            check[i] = (check[i] << utiBit) | (buf[(8 / utiBit) * 3 * i + 3 * j] & MASK);

    if (strcmp(check, "HBL") != 0)
    {
        printf("No hidden message found.\n");
        return NULL;
    }
    for (int i = 0; i < 4; i++)
        for (int j = (8 / utiBit) - 1; j >= 0; j--)
            check[i] =
                (check[i] << utiBit) | (buf[(8 / utiBit) * 3 * i + 3 * j + 3 * 3 * (8 / utiBit)] &
                                        MASK); // 3 * (8 / utiBit) is added to skip HBL bytes
    int len = 0;
    memcpy(&len, check, 4);

    char *result = (char *)malloc(len + sizeof(int));
    memset(result, 0, len);
    memcpy(result, &len, sizeof(int));
    for (int i = 0; i < len; i++)
        for (int j = (8 / utiBit) - 1; j >= 0; j--)
            result[i + sizeof(int)] =
                (result[i + sizeof(int)] << utiBit) |
                (buf[(8 / utiBit) * 3 * i + 3 * j + 3 * 7 * (8 / utiBit)] & MASK);

    free(buf);
    free(check);

    return result;
}
