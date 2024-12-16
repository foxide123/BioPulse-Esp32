#include "uri_handler.h"

static int custom_uri_encode(const char *src, char *dst, size_t dst_size)
{
    char a, b;
    size_t i = 0;
    size_t j = 0;

    while(src[i] != '\0' && j < dst_size - 1) {
        if(src[i] == '%') {
            if(isxdigit(src[i+1]) && isxdigit(src[i+2])) {
                a = src[i+1];
                b = src[i+2];
                a = (a >= 'a') ? (a - 'a' + 10) : (a >= 'A') ? (a - 'A' + 10) : (a - '0');
                b = (b >= 'a') ? (b - 'a' + 10) : (b >= 'A') ? (b - 'A' + 10) : (b - '0');
                dst[j++] = 16 * a + b;
                i += 3;
                continue;
            }
        } else if (src[i] == '+') {
            dst[j++] = ' ';
            i++;
            continue;
        }
        dst[j++] = src[i++];
    }

    dst[j] = '\0';

    if(src[i] != '\0') {
        return -1;
    }
    return (int)j;
}