#ifndef __FB_BASE
#define __FB_BASE

#define FBDEF static

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;
typedef float    f32;
typedef double   f64;

typedef struct {
    u64 length;
    u8* str;
} String;

#define U8(s) (u8 *)s

FBDEF String read_file(u8 *path, u8 *mode);

// file IO
String read_file(u8 *path, u8 *mode) {
    FILE *fp = fopen(path, mode);
    assert(fp);
    fseek(fp, 0, SEEK_END);
    size_t length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    u8 *buf = (u8*)malloc(length);
    fread(buf, length, 1, fp);
    fclose(fp);

    return (String) {
        .length = length,
        .str = buf
    };
}

// todo: rewrite this to fully understand what it's doing
void itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 2 + '0';   /* get next digit */
    } while ((n /= 2) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';

    {
        int i, j;
        char c;
        for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
            c = s[i];
            s[i] = s[j];
            s[j] = c;
        }
    }
}

#endif

