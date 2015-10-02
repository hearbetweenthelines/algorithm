#include "lsb.h"
#include <math.h>

int simple_write_wave(char const *filename, char *memdata, WAVE_INFO const *wave_info)
{
    FILE *file = fopen(filename, "w");
    if (!file)
        return -1;

    double zeroline = pow(2.0, wave_info->bitDepth - 1);
    int dataWidth   = wave_info->bitDepth / 8;
    int frameCount  = wave_info->dataSize / dataWidth;

    char *buf = (char *)malloc(sizeof(char) * (wave_info->dataSize + 44));
    if (!buf)
    {
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
    fwrite(buf, sizeof(char), wave_info->dataSize, file);
    fclose(file);
    free(buf);

    return 0;
}

int lsb_stego(char const *msg, int msglen, double **data, const WAVE_INFO *wave_info,
              char const *filename)
{
    if (msglen * 4 > wave_info->dataSize)
        return MSG_TOO_LONG;

    int dataWidth   = wave_info->bitDepth / 8;
    int frameCount  = wave_info->dataSize / dataWidth;
    double zeroline = pow(2.0, wave_info->bitDepth - 1);

    char *idata = (char *)malloc(frameCount * dataWidth);
    for (int i = 0; i < frameCount; i++)
    {
        int sample = (int)(data[i % wave_info->channels][i / wave_info->channels] * zeroline);

        if (msglen * 4 <= i)
        {
            sample &= ~0x3;
            sample |= (msg[i / 4] >> ((i % 4) * 2)) & 0x3;
        }
        memcpy(idata + i * dataWidth, &sample, dataWidth);
    }

    simple_write_wave(filename, idata, wave_info);

    free(idata);
    return 0;
}

char * lsb_destego(char const* audiopath)
{
    WAVE_INFO wave_info;
    int r = open_wave(audiopath, &wave_info);
    if (r == FILE_OPEN_ERROR || r == WAVE_NOT_MATCH || r == FMT_NOT_MATCH || r == DATA_NOT_FOUND)
        return NULL;
    return null;
}
