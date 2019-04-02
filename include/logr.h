#ifndef LOGR_LOGR_H
#define LOGR_LOGR_H

#define VERSION "0.4.2"
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


void PrintHelp(char *prog);
int ParseArg(int argc, char *argv[]);
void ReadLine();
void ReadPipe();
#endif