#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include "logr.h"
#include "filter.h"
#include "format.h"
#include "output.h"


static FILE* logfileList[MAX_LOGFILE];

void PrintHelp(char *prog) {
    printf("用法: %s [参数]... [文件]...\n", prog);
    printf("格式化日志。版本号：%s\n", VERSION);
    printf("  -h, --help           显示帮助内容\n");
    printf("\n");
    printf("  -j, --json           输出JSON格式\n");
    printf("  -J                   输出JSON格式，不处理类型为JSON的值\n");
    printf("\n");
    printf("  -c, --column         过滤列。多个列使用英文逗号分隔。内置列：\n");
    printf("	                     time (日志生成时间): %s\n", COL_TIME);
    printf("	                     level(日志等级)    : %s\n", COL_LEVEL);
    printf("	                     file (来源文件)    : %s\n", COL_FILE);
    printf("	                     logid(日志ID)     : %s\n", COL_LOGID);
    printf("	                     extra(其他)       : %s\n", COL_EXTRA);
    printf("\n");
    printf("  -f, --filter         过滤日志。格式如下：\n");
    printf("                         key*val   指定字段中，任意位置模糊查找\n");
    printf("                         key!*val  (取反)指定字段中，任意位置模糊查找\n");
    printf("                         key~val   指定字段中，正则查找\n");
    printf("                         key!~val  (取反)指定字段中，正则查找\n");
    printf("                         key>val   指定字段中，数值大于val\n");
    printf("                         key<val   指定字段中，数值小于val\n");
    printf("                         key>=val  指定字段中，数值大于等于val\n");
    printf("                         key<=val  指定字段中，数值小于等于val\n");
    printf("                         key!=val  (取反)指定字段中，数值不等于val\n");
    printf("                         key<>val  (取反)指定字段中，数值不等于val\n");
    printf("                         >val      任意字段数值大于val\n");
    printf("                         >=val     任意字段数值大于等于val\n");
    printf("                         <val      任意字段数值小于val\n");
    printf("                         <=val     任意字段数值小于等于val\n");
    printf("                         *val      任意字段模糊查找\n");
    printf("                         ~val      正则查找\n");
    printf("\n");
    printf("示例：\n");
    printf("tail -f ral-worker.log | logr -c t,uri -f 'cost>1000' -f 'uri~^bizas'\n");
    printf("tail -f ral-worker.log | logr -c t -c uri -f 'cost>1000' -f 'uri~^bizas'\n");
    printf("    表示仅显示日志时间和接口地址两列，并且只显示耗时大于1000，并且接口路径以bizas开始的日志\n");
}

void PrintVersion(char *prog) {
    printf("%s 版本号 %s\n", prog, VERSION);
}

/**
 * 解析参数
 *
 * @param argc
 * @param argv
 * @return
 */
int ParseArg(int argc, char *argv[]) {
    int c;
    int helpflg = 0, verflg = 0, errflg = 0, debug = 0, outputtype = OUTPUT_STRING;

    struct option longopts[] =
            {
                    {"column", 1, 0,           'c'},
                    {"filter", 1, 0,           'f'},
                    {"help",   0, &helpflg,    'h'},
                    {"json",   0, &outputtype, 'j'},
                    {"version",0, &verflg,     'v'},
                    {0,        0, 0,        0}
            };

    while ((c = getopt_long(argc, argv, "jJvhc:f:t:kcdD", longopts, NULL)) != EOF) {
        switch (c) {
            case 'h':
                helpflg = 1;
                break;
            case 'c':
                collect_colmun(optarg);
                break;
            case 'f':
                collect_filter(optarg);
                break;
            case 'v':
                verflg = 1;
                break;
            case 'j':
                outputtype = OUTPUT_JSON;
                break;
            case 'J':
                outputtype = OUTPUT_JSON_NOREC;
                break;
            case '?':
                errflg++;
                break;
            default:
//                printf("the option is%c--->%d, the argu is %s\n", c, c, optarg);
                break;
        }
    }

    if (helpflg || errflg) {
        PrintHelp(PROGRAM);
        exit(0);
    }
    if (verflg) {
        PrintVersion(PROGRAM);
        exit(0);
    }

    // 剩余参数认为是文件
    if (argc - optind > MAX_LOGFILE) {
        printf("文件过多");
        exit(1);
    }
    int i = 0, index = 0;
    int errCnt = 0;
    for (i = optind; i < argc; i++) {
        index = i - optind;
//        printf("file %d => %s\n", i, argv[i]);
        if (! (logfileList[index] = fopen(argv[i], "r"))) {
            printf("文件打开失败 %s\n", argv[i]);
            errCnt ++;
        }
    }
    if (errCnt > 0) {
        for (int j = 0; j < MAX_LOGFILE; ++j) {
            if (logfileList[i]) {
                fclose(logfileList[i]);
            }
        }
        exit(1);
    }

    format_init();

    if (i > optind)
        ReadLine(outputtype);
    else
        ReadPipe(outputtype);

    format_free();
    filter_free();

    return 1;
}

/**
 * 从文件中读取日志
 */
void ReadLine(const int outputtype) {
    char linebuf[MAX_LINE + 1];
    int nullCnt = 0;
    int i = 0, openedCnt = 0;
    unsigned long line = 0;

    do {
        openedCnt = 0;
        for (i = 0; i < MAX_LOGFILE; i++) {
            if (NULL != logfileList[i]) {
                openedCnt++;
                if (fgets(linebuf, MAX_LINE, logfileList[i])) {
                    format(linebuf, ++line, outputtype);
                } else {
                    fclose(logfileList[i]);
                    logfileList[i] = NULL;
                    nullCnt ++;
                }
            }
        }
    } while (openedCnt > 0 && nullCnt < openedCnt);
}

/**
 * 从管道中读取日志
 */
void ReadPipe(const int outputtype) {
    char buftrans_in[MAX_LINE + 1];
    unsigned long count = 0;

    while (fgets(buftrans_in , MAX_LINE , stdin))
        format(buftrans_in, ++count, outputtype);
}


int main(int argc, char *argv[]) {
    ParseArg(argc, argv);
}