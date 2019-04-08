//
// Created by Administrator on 2019/3/26.
//

#ifndef LOGR_UTILS_H
#define LOGR_UTILS_H

#include <stdio.h>
#include <string.h>
#include <sys/cdefs.h>

#define is_end(chr)  (*((char *)chr)=='\0'||*((char *)chr)=='\n'||*((char *)chr)=='\r')
#define is_eof(chr)  (*((char *)chr)=='\0')
#define is_spc(strchr)  (*((char *)strchr)==' ')
#define is_spcs(strchr)  (*((char *)strchr)==' '||*((char *)strchr)=='\n'||*((char *)strchr)=='\r')

#define STREQ(a, b)     (strcmp (a, b) == 0)

extern int should_colorize (void);

extern size_t concat(char*, ...);

void urldecode2(char *, const char *);
extern char * strstri(const char *, const char *) __THROW __attribute_pure__ __nonnull ((1, 2));
extern char * strrstri(const char *, const char *) __THROW __attribute_pure__ __nonnull ((1, 2));
extern int strileft(const char *, const char *) __THROW __attribute_pure__ __nonnull ((1, 2));
extern int striright(const char *, const char *) __THROW __attribute_pure__ __nonnull ((1, 2));
extern char *stristr(const char *, const char *) __THROW __attribute_pure__ __nonnull ((1, 2));

#endif //LOGR_UTILS_H