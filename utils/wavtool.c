#include <string.h>
#include "wavtool.h"

#define OUTPUT_OPEN_DEBUG 0
#define OUTPUT_WRITE_DEBUG 0

int open_wave(const char *filename, WAVE_INFO *wave_info)
{
    wave_info->filename = filename;
    char datacheck[5];
    int hasAddition;
    memset(datacheck, 0, 5);
    FILE *file = fopen(filename, "r");
    if (file == NULL)
        return FILE_OPEN_ERROR;

    // Check RIFF WAVE CHUNK
    fread(datacheck, sizeof(char), 4, file);
    if (strcmp("RIFF", datacheck) != 0)
        return RIFF_NOT_MATCH;
    fseek(file, 4, SEEK_CUR);
    fread(datacheck, sizeof(char), 4, file);
    if (strcmp("WAVE", datacheck) != 0)
        return WAVE_NOT_MATCH;

    // Check FORMAT CHUNK
    fread(datacheck, sizeof(char), 4, file);
    if (strcmp("fmt ", datacheck) != 0)
        return FMT_NOT_MATCH;
    fread(&hasAddition, sizeof(int), 1, file);
    fseek(file, 2, SEEK_CUR);
    fread(&(wave_info->channels), sizeof(short), 1, file);
    fread(&(wave_info->sampleRate), sizeof(int), 1, file);
    int temprate;
    fread(&temprate, sizeof(int), 1, file);
    wave_info->bitrate = temprate / 128.0;
    fseek(file, 2, SEEK_CUR);
    fread(&(wave_info->bitDepth), sizeof(short), 1, file);
    if (hasAddition == 18)
        fseek(file, 2, SEEK_CUR);

    // Find DATA CHUNK
    fread(datacheck, sizeof(char), 4, file);
    int i = 0;
    while (strcmp("data", datacheck) != 0 && i < 128)
    {
        fread(datacheck, sizeof(char), 4, file);
        i++;
    }
    if (strcmp("data", datacheck) != 0)
        return DATA_NOT_FOUND;
    fread(&(wave_info->dataSize), sizeof(int), 1, file);
    wave_info->data_align = (char)(ftell(file) & 0xFF);
    fclose(file);

#if OUTPUT_OPEN_DEBUG
    printf("----------DEBUG----------\nChannel: %d\n", wave_info->channels);
    printf("Sample rate: %d\n", wave_info->sampleRate);
    printf("Bitrate: %f\n", wave_info->bitrate);
    printf("Bitdepth: %d\n", wave_info->bitDepth);
    printf("Data Size: %d\n", wave_info->dataSize);
    printf("Data Start at 0x%X\n", (int)wave_info->data_align);
#endif

    return 0;
}

double **wave_read(WAVE_INFO *wave_info, unsigned int blanksec)
{
    double zeroline    = pow(2.0, wave_info->bitDepth - 1);
    int dataWidth      = wave_info->bitDepth / 8;
    int frameCount     = wave_info->dataSize / dataWidth;
    int chnlFrameCount = frameCount / wave_info->channels;
    int diff           = 32 - wave_info->bitDepth;
    double **result = (double **)malloc(sizeof(double *) * wave_info->channels);
    if (!result)
        return NULL;
    for (int chnl = 0; chnl < wave_info->channels; chnl++)
    {
        double *chnldata =
            (double *)malloc(sizeof(double) * (chnlFrameCount + blanksec * wave_info->sampleRate));
        if (!chnldata)
            return NULL;
        memset(chnldata + chnlFrameCount, 0, blanksec * wave_info->sampleRate);
        result[chnl] = chnldata;
    }


    FILE *file = fopen(wave_info->filename, "r");
    if (file == NULL)
        return NULL;

    // Load data into memory
    fseek(file, (long)wave_info->data_align, SEEK_SET);
    char *buf = (char *)malloc(wave_info->dataSize);
    fread(buf, sizeof(char), wave_info->dataSize, file);
    fclose(file);

    wave_info->dataSize += blanksec * wave_info->sampleRate * wave_info->channels * dataWidth;

    if (wave_info->bitDepth == 32)
    {
        for (int i = 0; i < frameCount; i++)
        {
            float temp;
            memcpy(&temp, buf + i * sizeof(float), sizeof(float));
            result[i % wave_info->channels][i / wave_info->channels] = (double)temp;
            // memcpy(result[i % wave_info->channels] + (i / wave_info->channels),
            // buf + i * sizeof(float), sizeof(float));
        }
    }
    else if (wave_info->bitDepth == 8)
    {
        int temp;
        for (int i = 0; i < frameCount; i++)
        {
            memcpy(&temp, buf + i, sizeof(char));
            temp += 0x80;
            temp &= 0xFF;
            temp <<= diff;
            temp >>= diff;
            result[i % wave_info->channels][i / wave_info->channels] = (double)temp / zeroline;
        }
    }
    else
    {
        int temp;
        for (int i = 0; i < frameCount; i++)
        {
            memcpy(&temp, buf + i * dataWidth, dataWidth);
            temp <<= diff;
            temp >>= diff;
            result[i % wave_info->channels][i / wave_info->channels] = (double)temp / zeroline;
        }
    }
    free(buf);
    return result;
}

int wave_write(const char *filename, const WAVE_INFO *wave_info, double **data)
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
// free(chardata);

#if OUTPUT_WRITE_DEBUG
    for (int i = 0; i < 44; i++)
    {
        printf("%02hhX ", *(buf + i));
        if (i % 16 == 15)
            printf("\n");
    }
    printf("\n");
#endif

    if (wave_info->bitDepth == 32)
    {
        for (int i = 0; i < frameCount; i++)
            start[i * sizeof(float)] =
                (float)data[i % wave_info->channels][i / wave_info->channels];
    }
    else if (wave_info->bitDepth == 8)
    {
        int temp;
        for (int i = 0; i < frameCount; i++)
        {
            temp = (int)(data[i % wave_info->channels][i / wave_info->channels] * zeroline);
            temp -= 0x80;
            temp &= 0xFF;
            memcpy(start + i, &temp, sizeof(char));
        }
    }
    else
    {
        int temp;
        for (int i = 0; i < frameCount; i++)
        {
            temp = (int)(data[i % wave_info->channels][i / wave_info->channels] * zeroline);
            memcpy(start + i * dataWidth, &temp, dataWidth);
        }
    }

    if (fwrite(buf, sizeof(char), wave_info->dataSize, file) != wave_info->dataSize)
    {
        free(buf);
        fclose(file);
        return -1;
    }
    fclose(file);

    free(buf);
    return 0;
}

void fill_default_wave_info(WAVE_INFO *wave_info, unsigned int sampleCount)
{
    wave_info->dataSize   = sampleCount * 4;
    wave_info->bitDepth   = 32;
    wave_info->channels   = 1;
    wave_info->sampleRate = 44100;
}
