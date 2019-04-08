//
// Created by Administrator on 2019/3/16.
//

#ifndef LOGR_OUTPUT_H
#define LOGR_OUTPUT_H

#include "format.h"

#define OUTPUT_STRING 1
#define OUTPUT_JSON 2
#define OUTPUT_JSON_NOREC 3

extern char outputstr[];

extern int print_log(const Log *log, int type);

#endif //LOGR_OUTPUT_H
