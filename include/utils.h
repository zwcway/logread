//
// Created by Administrator on 2019/3/26.
//

#ifndef LOGR_UTILS_H
#define LOGR_UTILS_H

#define is_end(chr)  (*(chr)=='\0'||*(chr)=='\n'||*(chr)=='\r')
#define is_spc(strchr)  (*(strchr)==' ')
#define is_spcs(strchr)  (*(strchr)==' '||*(strchr)=='\n'||*(strchr)=='\r')

void urldecode2(char *, const char *);

#endif //LOGR_UTILS_H
