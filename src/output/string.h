//
// Created by Administrator on 2019/4/3.
//

#ifndef LOGR_STRING_H
#define LOGR_STRING_H

#include "format.h"
#include "output.h"
#include "highlight.h"

#define P_STR(__s, key, str)        sprtf_key_val(__s, key, str, true);
/* 字符串输出至缓存 */
#define P_STR_BUF(__s, str)         SPRTF_STR_BUF(__s, "%s", str);
/* 包含引号的输出至缓存 */
#define P_STRQ_BUF(__s, str)        SPRTF_STR_BUF(__s, "\"%s\"", str);
/* 包含引号和冒号的输出至缓存 */
#define P_STRQJ_BUF(__s, str)       SPRTF_STR_BUF(__s, "\"%s\":", str);

extern void print_json_to_str(OutputBuffer*, cJSON *);
extern int print_log_to_str(OutputBuffer *, const Log *);

#endif //LOGR_STRING_H
