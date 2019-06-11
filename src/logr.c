#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>

#include "logr.h"
#include "filter.h"
#include "format.h"
#include "output.h"
#include "same-inode.h"
#include "highlight.h"


static FILE* logfileList[MAX_LOGFILE];

static struct stat out_stat;

int color_option = 0;
bool dev_null_output = false;
bool debug_flag = false;
int output_type = 0;
int output_option = 0;
char *logr_op = "=";
char *logr_spc = " ";

static bool use_file = false;

void ReadLine();
void ReadPipe();

#define CUNDER(__str)       COLOR(__str, C_UNDERLINE)

void onExit(int no) {
    format_free();
    filter_free();
    color_dict_free();
    exit(no);
}

void PrintVersion(char *prog) {
    printf("%s 版本号 %s\n", prog, VERSION);
    printf("项目地址：https://git.afpai.com/zhaoweichen/logread\n");
}

void PrintHelp(char *prog) {
    printf("用法: %s [参数]... [文件]...\n", prog);
    printf("格式化日志。\n");
    PrintVersion(prog);
    printf("\n");
    printf("  -h, --help           显示帮助内容\n");
    printf("\n");
    printf("  -j, --json           输出JSON格式\n");
    printf("  -J                   输出JSON格式，不处理类型为JSON的值\n");
    printf("\n");
    printf("  -c, --column         过滤列。多个列使用英文逗号分隔。内置列：\n");
    printf("	                     time (日志生成时间): %s\n", COL_TIME);
    printf("	                     level(日志等级)    : %s\n", COL_LEVEL);
    printf("	                     file (来源文件)    : %s\n", COL_FILE);
    printf("	                     logid(日志ID)      : %s\n", COL_LOGID);
    printf("	                     extra(其他)        : %s\n", COL_EXTRA);
    printf("\n");
    printf("  -C                   所有列必须同时存在。用法同 %s 。\n", CUNDER("-c|--column"));
    printf("  -K                   只输出值不输出字段名称。\n");
    printf("  -d                   每个字段之间的分隔符。\n");
    printf("  -t,--table           【TODO】 以表格形式输出。必须同时指定参数 %s\n", CUNDER("-c|-C"));
    printf("                       同时自动指定参数 %s。\n", CUNDER("-K"));
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
    printf("环境变量：");
    printf("LOGR_COLORS            自定义颜色");
    printf("                       默认值：LOGR_COLORS='%s'", HL_COLORS_DEFAULT);
    printf("\n");
    printf("示例：\n");
    printf("tail -f ral-worker.log | logr -c t,uri -f 'cost>1000' -f 'uri~^bizas'\n");
    printf("tail -f ral-worker.log | logr -c t -c uri -f 'cost>1000' -f 'uri~^bizas'\n");
    printf("    表示仅显示日志时间和接口地址两列，并且只显示耗时大于1000，并且接口路径以bizas开始的日志\n");
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
    int helpflg = 0, verflg = 0, errflg = 0;
    output_type = OUTPUT_STRING;

    struct option longopts[] =
            {
                    {"debug"  ,no_argument,         0,              OPTION_DEBUG},
                    {"help",   no_argument,         &helpflg,       OPTION_HELP},
                    {"version",no_argument,         &verflg,        OPTION_VERSION},
                    {"column", required_argument,   0,              OPTION_COLUMN},
                    {"filter", required_argument,   0,              OPTION_FILTER},
                    {"json",   no_argument,         0,              OPTION_JSON},
                    {"table"  ,no_argument,         0,              OPTION_TABLE},
                    {0,        0,                   0,              0}
            };

    while ((c = getopt_long(argc, argv, "hvjJC:f:c:Kd:", longopts, NULL)) != EOF) {
        switch (c) {
            case OPTION_HELP:
            case 'h':
                helpflg = 1;
                break;
            case OPTION_COLUMN:
            case 'c':
                collect_colmun(optarg, FC_OR);
                break;
            case 'C':
                collect_colmun(optarg, FC_AND);
                break;
            case OPTION_FILTER:
            case 'f':
                collect_filter(optarg);
                break;
            case OPTION_VERSION:
            case 'v':
                verflg = 1;
                break;
            case 'd':
                logr_spc = optarg;
                break;
            case OPTION_JSON:
            case 'j':
                output_type = OUTPUT_JSON;
                break;
            case 'J':
                output_type = OUTPUT_JSON_NOREC;
                break;
            case 'K':
                output_option |= OUTPUT_OPT_NOKEY;
                break;
            case OPTION_DEBUG:
                debug_flag = true;
                break;
            case '?':
                errflg++;
                break;
            default:
                errflg++;
                break;
        }
    }

    // 检测输出设备，校验设备是否支持颜色
    bool possibly_tty = false;
    struct stat tmp_stat;
    if (fstat (STDOUT_FILENO, &tmp_stat) == 0) {
        if (S_ISREG (tmp_stat.st_mode))
            out_stat = tmp_stat;
        else if (S_ISCHR (tmp_stat.st_mode)) {
            struct stat null_stat;
            if (stat ("/dev/null", &null_stat) == 0 && SAME_INODE (tmp_stat, null_stat))
                dev_null_output = true;
            else
                possibly_tty = true;
        }
    }

    color_option = 1;//debug_flag || (possibly_tty && should_colorize() && isatty(STDOUT_FILENO));

    if (debug_flag) {
        fprintf(stderr, "possibly_tty: %d dev_null_output:%d color_option:%d\n", possibly_tty, dev_null_output, color_option);
    }

    if (helpflg || errflg) {
        PrintHelp(PROGRAM);
        onExit(0);
    }
    if (verflg) {
        PrintVersion(PROGRAM);
        onExit(0);
    }

    // 剩余参数认为是文件
    if (argc - optind > MAX_LOGFILE) {
        printf("文件过多，最多支持 %d 个文件。", MAX_LOGFILE);
        onExit(1);
    }
    int i = 0, index = 0;
    int errCnt = 0;
    for (i = optind; i < argc; i++) {
        use_file = true;
        index = i - optind;
        if (! (logfileList[index] = fopen(argv[i], "r"))) {
            printf("文件打开失败 %s\n", argv[i]);
            errCnt ++;
        }
    }
    if (errCnt > 0) {
        for (i = 0; i < MAX_LOGFILE; ++i) {
            if (NULL != logfileList[i]) {
                fclose(logfileList[i]);
            }
        }
        onExit(1);
    }

    return 1;
}

/**
 * 从文件中读取日志
 */
void ReadLine() {
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
                    format(linebuf, ++line);
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
    unsigned long count = 0;

    while (fgets(buftrans_in , MAX_LINE , stdin))
        format(buftrans_in, ++count);
}

void intHandler(int dummy) {
    onExit(0);
}

int main(int argc, char *argv[]) {

    // 拦截用户终止信号，做善后操作
    struct sigaction sa;
    sa.sa_handler = intHandler;
    sigaction(SIGINT, &sa, NULL);

    ParseArg(argc, argv);


    parse_logr_colors();

    format_init();

    if (use_file)
        ReadLine();
    else
        ReadPipe();


    onExit(0);
}