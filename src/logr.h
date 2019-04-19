#ifndef LOGR_LOGR_H
#define LOGR_LOGR_H

#include <stdbool.h>
#include <limits.h>

#define VERSION "0.5.3"
#define PROGRAM "logr"
/**
 * 单次读取一行的最大字符数
 */
#define MAX_LINE 1024000
/**
 * 能够支持的最多日志文件数量
 */
#define MAX_LOGFILE 2


#define COL_HOST  "h"
#define COL_LEVEL "l"
#define COL_TIME  "t"
#define COL_FILE  "f"
#define COL_LOGID "g"
#define COL_EXTRA "x"


extern int color_option;

/** Stdout 被指向 /dev/null.  */
extern bool dev_null_output;

extern bool debug_flag;

extern int output_type;
extern int output_option;

extern char *logr_op;
extern char *logr_spc;

/* 没有相应的短命名参数  */
enum
{
    DEBUG_OPTION = CHAR_MAX + 1,
};

#endif