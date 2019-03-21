
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "format.h"
#include "output.h"

void field_free(Log_field *f) {
    Log_field *next;

    while (f) {
        next = f->next;

        free(f->key);
        if (f->type == TYPE_STRING) free(f->val->valstring);
        if (f->type == TYPE_JSON) cJSON_Delete(f->val->valjson);
        free(f->val);
        free(f);
        f = next;
    }
}

void log_free(Log *log) {

    if (log->file) free(log->file);
    if (log->time) free(log->time);
    if (log->extra) free(log->extra);

    if (log->value) field_free(log->value);
}

static char* sub_str_trim(const char *str, size_t len, unsigned char trim) {
    size_t reallen;
    char *copy;
    char *src = (char *)str;

    if (len <= 0) return 0;

    reallen = strlen(src);
    if (reallen < len) {
        len = reallen;
    }

    if (trim) {
        while (*src == ' ') {
            src++;
            len--;
        }
        while (*(src + len - 1) == ' ' || *(src + len - 1) == '\n' || *(src + len - 1) == '\r') len--;
    }

    len++;

    if (!(copy = (char *) malloc(len))) return 0;

    if (*(src + len - 1) != '\0') len--;

    memcpy(copy, src, len);

    if (*(src + len) != '\0') *(copy + len) = '\0';

    return copy;
}

int is_end(char *line) {
    if (*line == '\0') return 1;
    if (*line == '\n') return 1;
    if (*line == '\r') return 1;
    return 0;
}

int has_op(char *line) {
    char *tmp = line;
    while (*tmp != '\0') {
        if (*tmp == OP_OPEN || *tmp == OP_CLOSE) return 1;
        tmp ++;
    }
    return 0;
}

int is_int(char *str) {

}

int parse_app(Log *log, char *line) {
    char *steper = line;
    char *start = 0, *end = 0, *tmp = 0, *key = 0;
    unsigned char stch = 0, inval = 0;
    int keyLen = 0, valLen = 0;
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
                    inval = 1;
                }

                PUSH(stack, *steper);
                break;
            case OP_CLOSE:
                // 重复时取最后一个
                while (*(steper + 1) == OP_CLOSE) steper++;

                POP(stack, &stch);

                if (STACK_IS_EMPTY(stack)) {
                    // 栈结束，表示一个完整的键值遍历完成
                    inval = 0;
                    end = steper;
                    if (start) {
                        valLen = end - start;
                        // 值
                        field->val->valstring = sub_str(start + 1, valLen - 1);
                        if (field->val->valstring == NULL) {
                            L_SET_TYPE(field, TYPE_NULL);
                        } else {
                            L_SET_TYPE(field, TYPE_STRING);
                        }

                        if (!is_end(steper + 1) && has_op(steper + 1))
                            L_ADD_FIELD(field);

                        valLen = 0;
                    }

                    start = end = tmp = 0;
                }
                break;
            default:
                if (STACK_IS_EMPTY(stack)) {

                }
                break;
        }
    } while(*(++steper) != '\0');

    //结尾存在字符串
    if (key) {
        log->extra = sub_trim(key, steper - key);
    }

    STACK_FREE(stack);
}


int format_ral(const char *log_line) {
}

int format_app(const char *log_line) {
    Log app_log;
    int colcnt = 0;
    char *log = (char *)log_line;
    char *stt1, *stt2, *tmp;

    // level
    stt1 = strstr(log, ":");
    tmp = sub_str(log, stt1 - log);
    app_log.level = cov_level_str(tmp);
    free(tmp); tmp = 0;
    colcnt++;

    // file
    stt2 = strstr(stt1, "[");
    log = strstr(stt2, "]");
    app_log.file = sub_str(stt2 + 1, log - stt2 - 1);
    colcnt++;

    // time
    app_log.time = sub_trim(stt1 + 1, stt2 - stt1 - 1);
    app_log.ts = 0;
    colcnt++;

    // extra
    app_log.extra = 0;

    // 跳过 ]
    log++;

    parse_app(&app_log, log);

    print_log(&app_log);

    log_free(&app_log);

    return colcnt;
}

void format(const char *log) {

    char tag[38];
    int colCnt = 0;

    strncpy(tag, log, 37);

    if (!(
            strcmp(tag, "TRACE: ") ||
            strcmp(tag, "DEBUG: ") ||
            strcmp(tag, "NOTICE:") ||
            strcmp(tag, "WARNIN:")
    )) {

        printf("%s", log);
        return;
    }

    if (strstr(tag, ":  ral-worker *"))
        colCnt = format_ral(log);
    else
        colCnt = format_app(log);

    if (!colCnt)
        printf("%s", log);
}
