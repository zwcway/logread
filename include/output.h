//
// Created by Administrator on 2019/3/16.
//

#ifndef LOGR_OUTPUT_H
#define LOGR_OUTPUT_H

#include "format.h"


#define LOGR_OP "="
#define LOGR_SPC " "


#define P_STR(key, str)  do { hl_key(key); hl_op(LOGR_OP); hl_str(str); printf(LOGR_SPC); } while(0)
#define P_INT(key, integer)  do { hl_key(key); hl_op(LOGR_OP); hl_int(integer); printf(LOGR_SPC); } while(0)

void print_log(const Log *log);

#endif //LOGR_OUTPUT_H
