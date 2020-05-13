//
// Created by Administrator on 2019/3/16.
//

#ifndef LOGR_OUTPUT_H
#define LOGR_OUTPUT_H

#include "format.h"
#include "logr.h"

// 输出字符串
#define OUTPUT_STRING       0x1
// 输出JSON
#define OUTPUT_JSON         0x2
// 输出JSON，但不解析值中包含的JSON
#define OUTPUT_JSON_NOREC   0x3

// 不输出键名，即按表的方式输出
#define OUTPUT_OPT_NOKEY        0b0000001
// 不输出表头
#define OUTPUT_OPT_NOTITLE      0b0000010
// 输出分隔符
#define OUTPUT_OPT_SEPARATOR    0b0000100
// 格式化成表格
#define OUTPUT_OPT_TABLE        0b0001000

typedef struct OutputBuffer {
    size_t offset;
    size_t length;
    char outputstr[MAX_LINE];
} OutputBuffer;


#define OT_BUF_INIT(__buf)  do { \
    *(__buf)->outputstr = 0; \
    (__buf)->offset = 0; \
    (__buf)->length = MAX_LINE; \
} while(0)

extern int print_log(const Log *log, int type, int opt);

#endif //LOGR_OUTPUT_H
