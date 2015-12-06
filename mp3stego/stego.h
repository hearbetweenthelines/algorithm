#ifndef __STEGO_H__
#define __STEGO_H__

int openFile(char *filename);
int compress_encrpty(char *pin);
int getBit(int current);
int performStego(char* filename);

#endif