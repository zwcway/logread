//
// Created by Administrator on 2019/4/3.
//

#ifndef LOGR_STRING_H
#define LOGR_STRING_H

#include "format.h"
#include "output.h"
#include "highlight.h"

#define P_STR(__s, key, str)  do { \
hl_key(__s, key); hl_op(__s, LOGR_OP); hl_str(__s, str); hl_spc(__s, LOGR_SPC); \
} while(0)

#define P_LONG(__s, key, integer)  do { \
hl_key(__s, key); hl_op(__s, LOGR_OP); hl_long(__s, integer); hl_spc(__s,LOGR_SPC); \
} while(0)

#define P_JKEY(__s, key)  do { \
hl_jqot(__s, "\"");hl_jkey(__s, key);hl_jqot(__s, "\"");hl_jclo(__s, ":"); \
} while(0)

#define P_JSTR(__s, val)  do { \
hl_jqot(__s, "\"");hl_jstr(__s, val);hl_jqot(__s, "\""); \
} while(0)

#define P_JDBL(__s, val)  do { \
hl_jstr(__s, val); \
} while(0)

#define P_SPC(__s, val)  do { \
hl_spc(__s, val); \
} while(0)

extern void print_json_to_str(char **, cJSON *);
extern int print_log_to_str(char **, const Log *);

#endif //LOGR_STRING_H
