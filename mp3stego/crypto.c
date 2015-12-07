#include "crypto.h"
#include "aes.h"
#include <string.h>

// Codes here are just for demostration
int m_encrypt(char **data, int len, char *pin, int pinlen)
{
    uint8_t iv[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    int nlen    = len % 16 == 0 ? len : len + 16 - len % 16;
    char *ndata = (char *)malloc(nlen);
    memset(ndata, 0, nlen);
    memcpy(ndata, *data, len);

    char *npin = (char *)malloc(16);
    memset(npin, 0, 16);
    memcpy(npin, pin, pinlen);

    free(*data);
    *data = (char *)malloc(nlen);
    AES128_CBC_encrypt_buffer(*data, ndata, nlen, npin, iv);

    return nlen;
}

int decrypt(char **data, int len, char *pin, int pinlen)
{

    uint8_t iv[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    int nlen    = len % 16 == 0 ? len : len + 16 - len % 16;
    char *ndata = (char *)malloc(nlen);
    memset(ndata, 0, nlen);
    memcpy(ndata, *data, len);
    
    char *npin = (char *)malloc(16);
    memset(npin, 0, 16);
    memcpy(npin, pin, pinlen);

    free(*data);
    *data = (char *)malloc(nlen);
    AES128_CBC_decrypt_buffer(*data, ndata, nlen, npin, iv);

    return nlen;
}