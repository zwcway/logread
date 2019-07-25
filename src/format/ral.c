//
// Created by Administrator on 2019/4/1.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <format.h>
#include "format/ral.h"
#include "stack.h"
#include "utils.h"

regex_t *reg_ral = 0;
regmatch_t __ral_pmatch[1];


static int has_op(char *line) {
    char *tmp = line;
    while (*tmp != '\0' && *tmp != OP_CLOSE) {
        if (*tmp == OP_EQUAL) return 1;
        tmp ++;
    }
    return 0;
}

static char* strdecode(char *string) {
    char *dest = (char*)malloc(strlen(string) + 1);
    urldecode2(dest, string);
    while (!is_end(string)) {
        if (*string == '+') *string = ' ';
        (*string)++;
    }

    return dest;
}

static int parse_ral(Log *log, const char *line) {
    char *steper = (char *)line;
    char *start = 0, *end = 0, *tmp = 0, *key = 0;
    int valtype = 0;
    unsigned long keyLen = 0, valLen = 0;
    int count = 0;
    Log_field *field;

    // 跳过起始空格
    while(*steper == OP_SPER) steper++;

    L_INIT_FIELD(field);
    log->value = field;

    key = steper;

    do {
        switch (*steper) {
            case OP_CLOSE:
            case '\r':
            case '\n':
            case OP_SPER:
                end = steper;
                // 跳过连续空格
                while (is_spc(steper)) steper++;

                if (start) {
                    valLen = end - start;
                    // 值
                    tmp = sub_str(start + 1, valLen - 1);
                    valtype = parse_field(field, tmp);
                    if (valtype == TYPE_STRING) {
                        tmp = strdecode(field->valstr->valstring);
                        free(field->valstr->valstring);
                        field->valstr->valstring = tmp;
                    }
                    count ++;

                    if (!is_end(steper + 1) && has_op(steper + 1)) {
                        if(!key) key = steper;
                        L_ADD_FIELD(field);
                    }
                }

                start = 0;
                break;
            case OP_EQUAL:
                if (key) {
                    if (has_spc(key, keyLen = steper - key)) {
                        break;
                    }
                    field->key = sub_str(key, keyLen);
                    key = 0;
                }

                // 重复时取最先的一个
                if (*(steper - 1) == OP_OPEN) continue;

                start = steper;

                break;
            default:
                break;
        }
    } while(*(++steper) != '\0');

    //结尾存在字符串
    if (key && !is_end(key)) {
        log->extra = sub_trim(key, steper - key);
    }

    return count;
}

FORMATER_DESTORY_FUNC(ral) {
    if (reg_ral) {
        regfree(reg_ral);
        free(reg_ral);
    }
}

FORMATER_INIT_FUNC(ral) {
    reg_ral = (regex_t *)malloc(sizeof(regex_t));
    if (regcomp(reg_ral,
                "^(([0-9]+\\.){3}[0-9]+: )?(NOTICE|WARNING|TRACE): [01][0-9]-[0-3][0-9] [0-9]{2}:[0-9]{2}:[0-9]{2}:  ral(-worker)? \\* [0-9]+ \\[([0-9a-zA-Z_/\\.-]+\\.cpp:[0-9]+)\\]",
                REG_EXTENDED)) {
        regfree(reg_ral);
        free(reg_ral);
        reg_ral = 0;
    }
}
/**
 *
 * @param log
 * @param log_line
 * @param lineno
 * @return
 */
FORMATER_PROC_FUNC(ral) {
    if (!reg_ral || regexec(reg_ral, log_line, 1, __ral_pmatch, 0) != REG_NOERROR)
        return FORMATER_FAILED;

    int colcnt = 0;
    char *logline = (char *)log_line;
    char *stt1, *stt2, *tmp;
    struct tm tm;

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

    stt1++;

    // time
    L_INIT_TIME(log);
    stt2 = strstr(stt1, ": ");
    log->time->str = sub_trim(stt1 + 1, stt2 - stt1 - 1);
    if (0 != strptime(log->time->str, "%m-%d %H:%M:%S", &tm))
        log->time->ts = mktime(&tm);
    else
        log->time->ts = 0;
    colcnt++;

    // file
    stt2 = strstr(stt2, "[");
    logline = strstr(stt2, "]");
    log->file = sub_str(stt2 + 1, logline - stt2 - 1);
    colcnt++;

    log->logid = 0;

    // extra
    log->extra = 0;

    log->line = log_line;

    // 跳过 ][
    logline = strstr(logline, "[");
    logline++;

    colcnt += parse_ral(log, logline);

    return colcnt;
}
