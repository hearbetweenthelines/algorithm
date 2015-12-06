// Ref: http://blog.chinaunix.net/uid-21977330-id-3976817.html
// -------------------
// | RIFF WAVE CHUNK |
// -------------------
// |   FORMAT_CHUNK  |
// -------------------
// |    FACK CHUNK   |
// |    (optional)   |
// -------------------
// |    DATA CHUNK   |
// -------------------

#ifndef __WAVTOOL_H__
#define __WAVTOOL_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

enum
{
    SUCCESS,
    FILE_OPEN_ERROR,
    RIFF_NOT_MATCH,
    WAVE_NOT_MATCH,
    FMT_NOT_MATCH,
    DATA_NOT_FOUND
};

typedef struct
{
    const char *filename;
    unsigned short channels;
    unsigned int sampleRate;
    float bitrate;
    unsigned short bitDepth;
    unsigned int dataSize;
    char data_align;
} WAVE_INFO;

int open_wave(const char *filename, WAVE_INFO *wave_info);
double **wave_read(WAVE_INFO *wave_info, unsigned int blanksec);
int wave_write(const char *filename, const WAVE_INFO *wave_info, double **data);
void fill_default_wave_info(WAVE_INFO *wave_info, unsigned int sampleCount);

#endif