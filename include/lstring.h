//
// Created by Administrator on 2019/3/25.
//

#ifndef LOGR_LSTRING_H
#define LOGR_LSTRING_H

#include <stdio.h>

typedef struct String {
    char *str;
    int len;
} String;

#define STR_CPY(dest, src, len) do { \
dest.str = src; \
dest.len = len; \
} while(0)

#define STR_NEW_P(string) do { \
string->str = 0; \
string->len = 0; \
} while(0)

#define STR_PRINTF(string) do { \
printf("%.*s", str.len, str.len); \
} while(0)

int strprintf(const String str);

#endif //LOGR_LSTRING_H
