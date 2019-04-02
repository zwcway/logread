//
// Created by Administrator on 2019/3/26.
//

#ifndef LOGR_UTILS_H
#define LOGR_UTILS_H

#define is_end(chr)  (*(chr)=='\0'||*(chr)=='\n'||*(chr)=='\r')
#define is_eof(chr)  (*(chr)=='\0')
#define is_spc(strchr)  (*(strchr)==' ')
#define is_spcs(strchr)  (*(strchr)==' '||*(strchr)=='\n'||*(strchr)=='\r')

void urldecode2(char *, const char *);
extern char * strstri(const char *, const char *) __THROW __attribute_pure__ __nonnull ((1, 2));
extern char * strrstri(const char *, const char *) __THROW __attribute_pure__ __nonnull ((1, 2));
extern int strileft(const char *, const char *) __THROW __attribute_pure__ __nonnull ((1, 2));
extern int striright(const char *, const char *) __THROW __attribute_pure__ __nonnull ((1, 2));
extern char *stristr(const char *, const char *) __THROW __attribute_pure__ __nonnull ((1, 2));

#endif //LOGR_UTILS_H
