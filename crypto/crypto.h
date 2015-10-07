#ifndef __CRYPTO_H__
#define __CRYPTO_H__

#include <stdio.h>
#include <stdlib.h>

int encrypt(char **data, int len, void *pin);
int decrypt(char **data, int len, void *pin);

#endif
