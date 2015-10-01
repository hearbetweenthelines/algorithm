#ifndef __LSB_H__
#define __LSB_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utils/wavtool.h"

enum
{
    MSG_TOO_LONG
};

int lsb_stego(char const *msg, int msglen, double **data, const WAVE_INFO *wave_info,
              char const *filename);

#endif
