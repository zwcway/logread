//
// Created by Administrator on 2019/3/25.
//

#ifndef LOGR_LSTRING_H
#define LOGR_LSTRING_H

#include <stdio.h>
#include "color.h"

typedef struct String {
    const char *str;
    int len;
} String;

#define STR_CPY(dest, src, len) do { \
dest.str = src; \
dest.len = len; \
} while(0)

/**
 * 字符串实例化
 */
#define STR_INIT_P(string) do { \
string = (String *)malloc(sizeof(String)); \
if (string) { \
    string->str = 0; \
    string->len = 0; \
} \
}while(0)

/** 字符串带颜色打印 */
#define STR_CPRINTF(string, color) printf(color"%.*s"C_NONE, (string).len, (string).str)


/** 字符串带颜色打印 指针 */
#define STR_CPRINTF_P(string, color) STR_CPRINTF(*(string), color)

/** 字符串打印 */
#define STR_PRINTF(string) STR_CPRINTF(string, "")
/** 字符串打印 指针 */
#define STR_PRINTF_P(string) STR_CPRINTF_P(string, "")

/** 字符串长度 */
#define STR_LEN(string) (string).len
/** 字符串长度 指针 */
#define STR_LEN_P(string) STR_LEN(*(string))

extern int str_eq (const String *__s1, const String *__s2, const unsigned incase)
__THROW __attribute_pure__ __nonnull ((1, 2));

extern String* strsub(const char *str, size_t len, unsigned char trim)
__THROW __attribute_pure__ __nonnull ((1));

#endif //LOGR_LSTRING_H
