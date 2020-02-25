//
// Created by Administrator on 2019/3/28.
//
#include <stdlib.h>
#include <ctype.h>
#include <utils.h>
#include <string.h>
#include <stdarg.h>

/* Return non-zero if we should highlight matches in output.  */
int
should_colorize (void)
{
    /* $TERM is not normally defined on DOS/Windows, so don't require
       it for highlighting.  But some programs, like Emacs, do define
       it when running Grep as a subprocess, so make sure they don't
       set TERM=dumb.  */
    char const *t = getenv ("TERM");
    return ! (t && strcmp (t, "dumb") == 0);
}

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

extern bool has_spc(const char *str, size_t len) {
    for (size_t i = 0; i < len; i++) {
        if (is_spc(str + i)) {
            return true;
        }
    }

    return false;
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
    char *s1, *s2;
    int ch1, ch2;

    if (is_eof(str2)) return ((char *) str1);

    while (!is_eof(cp)) {
        s1 = cp;
        s2 = (char*)str2;
        while (*s1 && *s2) {
            ch1 = *s1; ch2 = *s2;
            if (isascii(ch1) && isupper(ch1)) ch1 = tolower(ch1);
            if (isascii(ch2) && isupper(ch2)) ch2 = tolower(ch2);

            if (ch1 - ch2 == 0)
                s1++, s2++;
            else break;
            if (is_eof(s2)) return (cp);
        }
        cp++;
    }

    return (NULL);
}

extern size_t concat(char * __restrict __str, ...) {
    va_list ap;
    size_t len = 0;
    const char *temp;

    va_start(ap, __str);
    while ((temp = va_arg(ap, const char *)) != 0) len += strlen(temp);
    va_end(ap);

    if (len == 0) return 0;

    len++;

    if (!(__str = (char *)malloc(len))) return 0;

    va_start(ap, __str);
    while ((temp = va_arg(ap, const char *)) != 0) strcat(__str, temp);
    va_end(ap);

    return len;
}

/**
 * 通配符匹配
 * @param s
 * @param p
 * @return
 */
bool is_match(const char *s, const char *p)
{
    bool star = false;
    const char *str = s;
    const char *ptr = p;

    while (*str != '\0') {
        switch (*ptr) {
            case '?':
                break;
            case '*':
                star = true;
                s = str;
                p = ptr;
                while (*p == '*') {
                    p++;
                }
                if (*p == '\0') { return true; }
                str = s - 1;
                ptr = p - 1;
                break;
            default:
                if (*str != *ptr) {
                    if (!star) {
                        return false;
                    }
                    s++;
                    str = s - 1;
                    ptr = p - 1;
                }
        }
        str++;
        ptr++;
    }
    while (*ptr == '*') { ptr++; }
    return (*ptr == '\0');
}

int char_count(const char *str, const char c) {
    int num = 0;
    while (*str != '\0')
        if (*str++ == c)
            num++;

    return num;
}
