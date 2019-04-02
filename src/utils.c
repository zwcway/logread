//
// Created by Administrator on 2019/3/28.
//
#include <stdlib.h>
#include <ctype.h>
#include <utils.h>
#include <string.h>

void urldecode2(char *dst, const char *src) {
    char a, b;
    while (*src) {
        if ((*src == '%') &&
            ((a = src[1]) && (b = src[2])) &&
            (isxdigit(a) && isxdigit(b))) {
            if (a >= 'a')
                a -= 'a' - 'A';
            if (a >= 'A')
                a -= ('A' - 10);
            else
                a -= '0';
            if (b >= 'a')
                b -= 'a' - 'A';
            if (b >= 'A')
                b -= ('A' - 10);
            else
                b -= '0';
            *dst++ = (char)(16 * a + b);
            src += 3;
        } else if (*src == '+') {
            *dst++ = ' ';
            src++;
        } else {
            *dst++ = *src++;
        }
    }
    *dst++ = '\0';
}

char * strstri(const char *__haystack, const char *__needle) {
}

char * strrstri(const char *__haystack, const char *__needle) {
}

/**
 * 比较字符串是否是开头的子串
 *
 * @param __haystack 被查找的字符串
 * @param __needle   查找的字符串子串
 * @return
 */
int strileft(const char *__haystack, const char *__needle) {
    size_t len1 = strlen(__haystack), len2 = strlen(__needle);
    int ch1, ch2;
    if (len1 < len2) return 0;
    if (len2 == 0) return 1;

    while (!is_eof(__haystack)) {
        if (is_eof(__needle)) break;

        ch1 = *__haystack; ch2 = *__needle;
        if (isascii(ch1) && isupper(ch1)) ch1 = tolower(ch1);
        if (isascii(ch2) && isupper(ch2)) ch2 = tolower(ch2);

        if (ch1 != ch2) return 0;

        __haystack++;
        __needle++;
    }

    return 1;
}

/**
 * 比较字符串是否是结尾的子串
 *
 * @param __haystack 被查找的字符串
 * @param __needle   查找的字符串子串
 * @return
 */
int striright(const char *__haystack, const char *__needle) {
    size_t len1 = strlen(__haystack), len2 = strlen(__needle);
    int ch1, ch2;

    if (len1 == 0) return 0;
    if (len1 < len2) return 0;
    if (len2 == 0) return 1;

    const char *right1 = __haystack + len1 - 1;
    const char *right2 = __needle + len2 - 1;

    do {
        ch1 = *right1; ch2 = *right2;
        if (isascii(ch1) && isupper(ch1)) ch1 = tolower(ch1);
        if (isascii(ch2) && isupper(ch2)) ch2 = tolower(ch2);

        if (ch1 != ch2) return 0;
        if (right2 == __needle) break;

        right2--;
    } while (right1-- != __haystack);

    return 1;
}

char *stristr(const char *str1, const char *str2) {
    char *cp = (char *) str1;
    char *s1;
    int ch1, ch2;

    if (is_eof(str2)) return ((char *) str1);

    while (*cp) {
        s1 = cp;

        while (*s1 && *str2) {
            ch1 = *s1; ch2 = *str2;
            if (isascii(ch1) && isupper(ch1)) ch1 = tolower(ch1);
            if (isascii(*str2) && isupper(*str2)) ch2 = tolower(*str2);

            if (ch1 - ch2 == 0) s1++, str2++;
            else break;
        }

        if (is_eof(str2)) return (cp);
        cp++;
    }

    return (NULL);
}