#ifndef __STEGO_H__
#define __STEGO_H__

#include "../utils/wavtool.h"

int stego(char const *msg, int msglen, double **data, const WAVE_INFO *wave_info,
          char const *filename);
char* destego(char const* audiopath);

#endif