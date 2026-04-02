#include "woody.h"

static void swap(uint8_t *a, uint8_t *b)
{
    uint8_t tmp = *a;
    *a = *b;
    *b = tmp;
}

void rc4_ksa(uint8_t *s, uint8_t *key, size_t key_len)
{
    for (int i = 0; i < 256; i++)
        s[i] = i;

    int j = 0;
    for (int i = 0; i < 256; i++)
    {
        j = (j + s[i] + key[i % key_len]) & 0xFF;
        swap(&s[i], &s[j]);
    }
}


void rc4_prga(uint8_t *s, uint8_t *data, size_t data_len)
{
    int i = 0;
    int j = 0;

    for (size_t n = 0; n < data_len; n++)
    {
        i = (i + 1) & 0xFF;
        j = (j + s[i]) & 0xFF;
        swap(&s[i], &s[j]);
        
        uint8_t keystream_byte = s[(s[i] + s[j]) & 0xFF]; 
        data[n] ^= keystream_byte;
    }
}

void rc4_crypt(uint8_t *data, size_t data_len, uint8_t *key, size_t key_len)
{
    uint8_t s[256];

    rc4_ksa(s, key, key_len);
    rc4_prga(s, data, data_len);
}