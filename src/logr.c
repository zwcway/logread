#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include "logr.h"
#include "format.h"


static FILE* logfileList[MAX_LOGFILE];

static char g_deviceid[32] = {0};

static char g_serverIP[16] = "172.18.18.18";
static int g_serverPort = 8888;

void PrintHelp(char *prog) {
    printf("用法: %s [参数]... [文件]...\n", prog);
    printf("格式化日志。版本号：%s\n", VERSION);
    printf("  -h, --help           显示帮助内容\n");
    printf("\n");
    printf("  -c, --column         过滤列。多个列使用英文逗号分隔。内置列：\n");
    printf("	                     time (日志生成时间): %s\n", COL_TIME);
    printf("	                     level(日志等级)    : %s\n", COL_LEVEL);
    printf("	                     file (来源文件)    : %s\n", COL_FILE);
    printf("	                     logid(日志ID)     : %s\n", COL_LOGID);
    printf("	                     extra(其他)       : %s\n", COL_EXTRA);
    printf("\n");
    printf("  -f, --filter         过滤日志。多个条件使用逗号分隔。格式如下：\n");
    printf("                         key=val   指定字段中，任意位置模糊查找【待】\n");
    printf("                         key~val   指定字段中，正则查找【待】\n");
    printf("                         key>val   指定字段中，数值大于val【待】\n");
    printf("                         key<val   指定字段中，数值小于val【待】\n");
    printf("                         key>=val  指定字段中，数值大于等于val【待】\n");
    printf("                         key<=val  指定字段中，数值小于等于val【待】\n");
    printf("                         =val      任意位置模糊查找【待】\n");
    printf("                         ~val      正则查找【待】\n");
    printf("\n");
    printf("示例：\n");
    printf("tail -f ral-worker.log | logr -c t,uri -f 'cost>1000,uri~^bizas'\n");
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
    int helpflg = 0, verflg = 0, errflg = 0, debug = 0;

    struct option longopts[] =
            {
                    {"column", 1, 0,        'c'},
                    {"filter", 1, 0,        'f'},
                    {"help",   0, &helpflg, 'h'},
                    {"version",0, &verflg,  'v'},
                    {0,        0, 0,        0}
            };

    while ((c = getopt_long(argc, argv, "vhc:f:t:kcdD", longopts, NULL)) != EOF) {
        switch (c) {
            case 'h':
                helpflg = 1;
                break;
            case 'c':
                printf("%c => %s \n", c, optarg);
                break;
            case 'f':
                printf("%c => %s \n", c, optarg);
                break;
            case 'v':
                verflg = 1;
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
        printf("file %d => %s\n", i, argv[i]);
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
    if (i > optind)
        ReadLine();
    else
        ReadPipe();
}

/**
 * 从文件中读取日志
 */
void ReadLine() {
    char linebuf[MAX_LINE];
    int nullCnt = 0;
    int i = 0, openedCnt = 0;

    do {
        openedCnt = 0;
        for (i = 0; i < MAX_LOGFILE; i++) {
            if (NULL != logfileList[i]) {
                openedCnt++;
                if (fgets(linebuf, MAX_LINE - 1, logfileList[i])) {
                    format(linebuf);
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
void ReadPipe() {
    char buftrans_in[MAX_LINE + 1];

    while (fgets(buftrans_in , MAX_LINE , stdin))
        format(buftrans_in);
}


int main(int argc, char *argv[]) {
    ParseArg(argc, argv);
}