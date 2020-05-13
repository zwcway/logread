//
// Created by Administrator on 2019/3/26.
//

#ifndef LOGR_UTILS_H
#define LOGR_UTILS_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/cdefs.h>

#define is_nr(chr)  (*((char *)chr)=='\n'||*((char *)chr)=='\r')
#define is_eof(chr)  (*((char *)chr)=='\0')
#define is_end(chr)  (is_eof(chr)||(is_nr(chr)))
#define is_spc(strchr)  (*((char *)strchr)==' ')
#define is_spcs(strchr)  (is_spc(strchr)||is_nr(strchr))

#define STREQ(a, b)     (strcmp (a, b) == 0)

extern int should_colorize(void);

extern size_t concat(char *, ...);

void urldecode2(char *, const char *);

extern char *strstri(const char *, const char *) __THROW __attribute_pure__ __nonnull ((1, 2));

extern char *strrstri(const char *, const char *) __THROW __attribute_pure__ __nonnull ((1, 2));

extern int strileft(const char *, const char *) __THROW __attribute_pure__ __nonnull ((1, 2));

extern int striright(const char *, const char *) __THROW __attribute_pure__ __nonnull ((1, 2));

extern char *stristr(const char *, const char *) __THROW __attribute_pure__ __nonnull ((1, 2));

extern bool has_spc(const char *, size_t len) __THROW __attribute_pure__ __nonnull ((1));

extern bool is_match(const char *str, const char *pattern) __THROW __attribute_pure__ __nonnull ((1, 2));

extern int char_count(const char *str, const char c) __THROW __attribute_pure__ __nonnull ((1));

extern char *str_pad_left(const char *str, char pad, size_t len) __THROW __attribute_pure__ __nonnull ((1));

#endif //LOGR_UTILS_H
