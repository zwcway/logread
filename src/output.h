//
// Created by Administrator on 2019/3/16.
//

#ifndef LOGR_OUTPUT_H
#define LOGR_OUTPUT_H

#include "format.h"
#include "logr.h"

#define OUTPUT_STRING 1
#define OUTPUT_JSON 2
#define OUTPUT_JSON_NOREC 3

typedef struct OutputBuffer {
    size_t offset;
    size_t length;
    char outputstr[MAX_LINE];
} OutputBuffer;


#define OT_BUF_INIT(__buf)  do { \
    (__buf)->offset = 0; \
    (__buf)->length = MAX_LINE; \
} while(0)

extern int print_log(const Log *log, int type);

#endif //LOGR_OUTPUT_H
