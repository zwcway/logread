//
// Created by Administrator on 2019/4/1.
// 业务日志格式解析器
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "stack.h"
#include "format.h"
#include "output.h"
#include "type.h"
#include "utils.h"


regex_t *reg_app = 0;
regmatch_t __app_pmatch[1];


static int app_has_op(char *line) {
    char *tmp = line;
    while (*tmp != '\0') {
        if (*tmp == OP_OPEN || *tmp == OP_CLOSE) return 1;
        tmp ++;
    }
    return 0;
}

static int parse_app(Log *log, const char *line) {
    char *steper = (char *)line;
    char *start = 0, *end = 0, *tmp = 0, *key = 0;
    unsigned char stch = 0;
    unsigned long keyLen = 0, valLen = 0;
    int count = 0;
    Stack *stack;
    Log_field *field;

    // 跳过起始空格
    while(*steper == OP_SPER) steper++;

    STACK_INIT(stack);

    L_INIT_FIELD(field);
    log->value = field;

    key = steper;

    do {
        switch (*steper) {
            case OP_DQOUTE:
            case OP_QOUTE:
                break;
            case OP_SPER:
                // 跳过连续空格
                while (is_spc(steper)) steper++;

                if (STACK_IS_EMPTY(stack)) {
                    if(!key) key = steper;
                    else
                        while (is_spc(key)) key++;
                }
                break;
            case OP_OPEN:
                if(is_spc(steper - 1)) {
                    while(!is_nr(steper + 1)) steper++;
                    goto PARSE_EXTRA;
                }

                if (key) {
                    if (has_spc(key, keyLen = steper - key)) {
                        break;
                    }
                    field->key = sub_trim(key, keyLen);
                    key = 0;
                }

                // 重复时取最先的一个
//                if (*(steper - 1) == OP_OPEN) continue;

                if (STACK_IS_EMPTY(stack)) {
                    start = steper;
                }

                PUSH(stack, *steper);
                break;
            case '\r':
            case '\n':
            case OP_CLOSE:
                // 重复时取最后一个
//                while (*(steper + 1) == OP_CLOSE) steper++;

                POP(stack, &stch);

                if (STACK_IS_EMPTY(stack)) {
                    // 栈结束，表示一个完整的键值遍历完成
                    end = steper;
                    if (start) {
                        valLen = end - start;
                        // 值
                        tmp = sub_str(start + 1, valLen - 1);
                        parse_field(field, tmp);
                        count ++;

                        if (!is_end(steper + 1) && app_has_op(steper + 1))
                            L_ADD_FIELD(field);

                    }
                    //if(!key) key = steper;
                    start = 0;
                }
                break;
            default:
                break;
        }
    } while(!is_eof(++steper));

    PARSE_EXTRA:
    //结尾存在字符串
    if (key && !is_end(key)) {
        log->extra = sub_trim(key, steper - key);
    }

    STACK_FREE(stack);

    return count;
}

FORMATER_DESTORY_FUNC(app) {
    if (reg_app) {
        regfree(reg_app);
        free(reg_app);
    }
}

FORMATER_INIT_FUNC(app) {
    reg_app = (regex_t *)malloc(sizeof(regex_t));
    if (regcomp(reg_app,
                "(.+?: )?(NOTICE|WARNING|TRACE|DEBUG): [0-9]{2}-[01][0-9]-[0-3][0-9] [0-9]{2}:[0-9]{2}:[0-9]{2} \\[([0-9a-zA-Z_/]+\\.php:[0-9]+)\\]",
                REG_EXTENDED)) {
        regfree(reg_app);
        free(reg_app);
        reg_app = 0;
    }
}

/**
 * TODO 使用词法分析器
 *
 * @param log_line
 * @return
 */
FORMATER_PROC_FUNC(app) {
    if (!reg_app || regexec(reg_app, log_line, 1, __app_pmatch, 0) != REG_NOERROR)
        return FORMATER_FAILED;

    int colcnt = 0;
    char *logline = (char *)log_line;
    char *stt1, *stt2, *tmp;

    log->pos = lineno;

    // level
    stt1 = strstr(logline, ":");
    tmp = sub_str(logline, stt1 - logline);

    if (isIpV4(tmp)) {
        L_INIT_HOST(log);
        log->host->ip = tmp;
        log->host->lip = 0;
        colcnt++;

        logline = ++stt1;

        stt1 = strstr(logline, ":");
        tmp = sub_trim(logline, stt1 - logline);
    }
    L_INIT_LEVEL(log);
    log->level->lstr = tmp;
    log->level->lint = cov_level_str(tmp);
    colcnt++;

    // file
    stt2 = strstr(stt1, "[");
    logline = strstr(stt2, "]");
    log->file = sub_str(stt2 + 1, logline - stt2 - 1);
    colcnt++;

    // time
    L_INIT_TIME(log);
    log->time->valstring = sub_trim(stt1 + 1, stt2 - stt1 - 1);
    Time time;
    strtotime(log->time->valstring, &time);
    if (strtotime(log->time->valstring, &time))
        log->time->vallong = time.ts;
    else
        log->time->vallong = 0;
    colcnt++;

    log->logid = 0;
    log->logidstr = 0;

    // extra
    log->extra = 0;

    // 跳过 ]
    logline++;

    log->line = log_line;

    colcnt += parse_app(log, logline);

    return colcnt;
}
