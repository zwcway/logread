//
// Created by Administrator on 2019/3/16.
//

#ifndef LOGR_OUTPUT_H
#define LOGR_OUTPUT_H

#include "format.h"
#include "highlight.h"


#define LOGR_OP "="
#define LOGR_SPC " "


#define P_STR(key, str)  do { hl_key(key); hl_op(LOGR_OP); hl_str(str); printf(LOGR_SPC); } while(0)
#define P_LONG(key, integer)  do { hl_key(key); hl_op(LOGR_OP); hl_long(integer); printf(LOGR_SPC); } while(0)
#define P_JKEY(key)  do { hl_jqot("\"");hl_jkey(key);hl_jqot("\"");hl_jclo(":"); } while(0)
#define P_JSTR(val)  do { hl_jqot("\"");hl_jstr(val);hl_jqot("\"");} while(0)
#define P_JDBL(val)  do { hl_jstr(val);} while(0)

int print_log(const Log *log);

#endif //LOGR_OUTPUT_H
