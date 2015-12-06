#include "stego.h"
#include "lsb.h"

int stego(char const *msg, int msglen, double **data, const WAVE_INFO *wave_info,
          char const *filename)
{
    lsb_stego(msg, msglen, data, wave_info, filename);
}

char* destego(char const* audiopath)
{
	return lsb_destego(audiopath);
}