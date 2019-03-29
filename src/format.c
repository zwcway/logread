
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include "stack.h"
#include "format.h"
#include "output.h"
#include "type.h"
#include "time.h"
#include "utils.h"

regex_t *reg_ral = 0, *reg_app = 0;

void field_free(Log_field *f) {
    Log_field *next;

    while (f) {
        next = f->next;

        free(f->key);
        if (f->type == TYPE_JSON)
            cJSON_Delete(f->val.valjson);
        else if (f->val.valstr) {
            if (f->val.valstr->valstring) free(f->val.valstr->valstring);
            free(f->val.valstr);
        }
        free(f);
        f = next;
    }
}

/**
 * 销毁 log
 * log初始化不是指针，因此不需要销毁log本身
 * @param log
 */
void log_free(Log *log) {

    if (log->level) {
        if (log->level->lstr) free(log->level->lstr);
        free(log->level);
    }
    if (log->time) {
        if (log->time->str) free(log->time->str);
        free(log->time);
    }
    if (log->host) {
        if (log->host->ip) free(log->host->ip);
        free(log->host);
    }
    if (log->file) free(log->file);
    if (log->extra) free(log->extra);

    if (log->value) field_free(log->value);
}

int has_op(char *line) {
    char *tmp = line;
    while (*tmp != '\0') {
        if (*tmp == OP_OPEN || *tmp == OP_CLOSE) return 1;
        tmp ++;
    }
    return 0;
}

void parse_field(Log_field *field, char *tmp) {
    int valtype = guessType(tmp);
    L_SET_TYPE(field, valtype);

    switch (valtype) {
        case TYPE_LONG:
            LF_LONG(field, tmp);
            break;
        case TYPE_DOUBLE:
            LF_DOUBLE(field, tmp);
            break;
        case TYPE_JSON:
            field->val.valjson = cJSON_Parse(tmp);
            if (field->val.valjson == 0) {
                L_SET_TYPE(field, TYPE_STRING);
                LF_STRING(field, tmp);
            }
            break;
        case TYPE_IP:
            LF_STRING(field, tmp);
            break;
        case TYPE_STRING:
            LF_STRING(field, tmp);
            break;
        default:
            free(tmp);
            break;
    }
}

int parse_app(Log *log, const char *line) {
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
    log->line = line;

    key = steper;

    do {
        switch (*steper) {
            case OP_SPER:
                // 跳过连续空格
                while (*(steper + 1) == OP_SPER) steper++;

                if (STACK_IS_EMPTY(stack)) {
                    if(!key) key = steper + 1;
                }
                break;
            case OP_OPEN:
                if (key) {
                    keyLen = steper - key;
                    field->key = sub_str(key, keyLen);
                    key = 0;
                }

                // 重复时取最先的一个
                if (*(steper - 1) == OP_OPEN) continue;

                if (STACK_IS_EMPTY(stack)) {
                    start = steper;
                }

                PUSH(stack, *steper);
                break;
            case OP_CLOSE:
                // 重复时取最后一个
                while (*(steper + 1) == OP_CLOSE) steper++;

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

                        if (!is_end(steper + 1) && has_op(steper + 1))
                            L_ADD_FIELD(field);

                    }

                    start = 0;
                }
                break;
            default:
                if (STACK_IS_EMPTY(stack)) {

                }
                break;
        }
    } while(*(++steper) != '\0');

    //结尾存在字符串
    if (key && !is_end(key)) {
        log->extra = sub_trim(key, steper - key);
    }

    STACK_FREE(stack);

    return count;
}

int format_ral(Log *ral_log, const char *log_line, const unsigned long lineno) {
    return 0;
}

/**
 * TODO 使用词法分析器
 *
 * @param log_line
 * @return
 */
int format_app(Log *app_log, const char *log_line, const unsigned long lineno) {
    int colcnt = 0;
    char *log = (char *)log_line;
    char *stt1, *stt2, *tmp;
    struct tm tm;

    app_log->pos = lineno;

    // level
    stt1 = strstr(log, ":");
    tmp = sub_str(log, stt1 - log);

    if (isIpV4(tmp)) {
        L_INIT_HOST(app_log);
        app_log->host->ip = tmp;
        app_log->host->lip = 0;
        colcnt++;

        log = ++stt1;

        stt1 = strstr(log, ":");
        tmp = sub_trim(log, stt1 - log);
    }
    L_INIT_LEVEL(app_log);
    app_log->level->lstr = tmp;
    app_log->level->lint = cov_level_str(tmp);
    colcnt++;

    // file
    stt2 = strstr(stt1, "[");
    log = strstr(stt2, "]");
    app_log->file = sub_str(stt2 + 1, log - stt2 - 1);
    colcnt++;

    // time
    L_INIT_TIME(app_log);
    app_log->time->str = sub_trim(stt1 + 1, stt2 - stt1 - 1);
    if (0 != strftime(app_log->time->str, strlen(app_log->time->str), "%y-%m-%d %H:%M:%S", &tm))
        app_log->time->ts = mktime(&tm);
    else
        app_log->time->ts = 0;
    colcnt++;

    app_log->logid = 0;

    // extra
    app_log->extra = 0;

    // 跳过 ]
    log++;

    colcnt += parse_app(app_log, log);

    return colcnt;
}

int format_normal(Log *log, const char *log_line, const unsigned long lineno) {
    return 0;
}

void format(const char *line, const unsigned long lineno) {
    int colCnt = 0;
    Log log;
    char tag[38];
    regmatch_t pmatch[1];

    int (*func)(Log*, const char*, const unsigned long) = 0;

    if (reg_ral && reg_app) {
        if (regexec(reg_ral, line, 1, pmatch, 0) == REG_NOERROR)
            func = format_ral;
        else if (regexec(reg_app, line, 1, pmatch, 0) == REG_NOERROR)
            func = format_app;
        else func = format_normal;
    }
    else {
        strncpy(tag, line, 37);

        if (
                0 == strstr(tag, "TRACE: ") ||
                0 == strstr(tag, "DEBUG: ") ||
                0 == strstr(tag, "NOTICE:") ||
                0 == strstr(tag, "WARNIN:")
                ) {
            func = format_normal;
        } else if (strstr(tag, ":  ral-worker *")) {
            func = format_ral;
        } else {
            func = format_app;
        }
    }

    L_INIT_LOG(&log);

    colCnt = (*func)(&log, line, lineno);

    if (!colCnt) {
        printf("%s", line);
    } else {
        print_log(&log);
    }

    log_free(&log);
}

static unsigned char cov_level_str(char *str) {
    if (!strcmp(str, LEVEL_STR_DEBUG)) return LEVEL_DEBUG;
    if (!strcmp(str, LEVEL_STR_TRACE)) return LEVEL_TRACE;
    if (!strcmp(str, LEVEL_STR_NOTICE)) return LEVEL_NOTICE;
    if (!strcmp(str, LEVEL_STR_WARNING)) return LEVEL_WARNING;
    if (!strcmp(str, LEVEL_STR_ERROR)) return LEVEL_ERROR;
    return LEVEL_UNKNOwN;
}

void format_init(void) {
    if (0 == reg_ral) {
        reg_ral = (regex_t *)malloc(sizeof(regex_t));
        if (regcomp(reg_ral,
                    "^(([0-9]+\\.){3}[0-9]+: )?(NOTICE|WARNING|TRACE): [01][0-9]-[0-3][0-9] [0-9]{2}:[0-9]{2}:[0-9]{2}:  ral(-worker)? \\* [0-9]+ \\[([0-9a-zA-Z_/]+\\.cpp:[0-9]+)\\]",
                    REG_EXTENDED)) {
            regfree(reg_ral);
            free(reg_ral);
            reg_ral = 0;
        }
    }
    if (0 == reg_app) {
        reg_app = (regex_t *)malloc(sizeof(regex_t));
        if (regcomp(reg_app,
                    "^(([0-9]+\\.){3}[0-9]+: )?(NOTICE|WARNING|TRACE|DEBUG): [0-9]{2}-[01][0-9]-[0-3][0-9] [0-9]{2}:[0-9]{2}:[0-9]{2} \\[([0-9a-zA-Z_/]+\\.php:[0-9]+)\\]",
                    REG_EXTENDED)) {
            regfree(reg_app);
            free(reg_app);
            reg_app = 0;
        }
    }
}
void format_free(void) {
    if (reg_ral) {
        regfree(reg_ral);
        free(reg_ral);
    }
    if (reg_app) {
        regfree(reg_app);
        free(reg_app);
    }
}