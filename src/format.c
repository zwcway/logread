
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

#include "format/app.h"
#include "format/ral.h"

static Formater formaters[] = {
        ADD_FORMATER(ral),
        ADD_FORMATER(app)
};
static int formaterlen = 0;

void field_hl(Highlight * hl) {
    if (hl) {
        if (hl->pre) free(hl->pre);
        if (hl->str) free(hl->str);
        if (hl->app) free(hl->app);
        free(hl);
    }
}

void field_free(Log_field *f) {
    Log_field *next;

    while (f) {
        next = f->next;

        if (f->valjson) {
            cJSON_Delete(f->valjson);
            f->valjson = 0;
        }
        if (f->valstr) {
            if (f->valstr->valstring) {
                free(f->valstr->valstring);
                f->valstr->valstring = 0;
            }
            free(f->valstr);
            f->valstr = 0;
        }

        field_hl(f->hl);
        free(f->key);
        f->key = 0;

        free(f);
        f = next;
    }
}

Log_field* field_duplicate(Log_field *f) {
    Log_field *copy;

    L_INIT_FIELD(copy);

    memcpy(copy, f, sizeof(Log_field));

    copy->key = strdup(f->key);

    L_INIT_VALUE(copy);
    copy->valstr->valstring = strdup(f->valstr->valstring);

    copy->next = copy->prev = NULL;
    return copy;
}

/**
 * 销毁 log
 * log初始化不是指针，因此不需要销毁log本身
 * @param log
 */
void log_free(Log *log) {

    if (log->level) {
        if (log->level->lstr) free(log->level->lstr);
        field_hl(log->level->hl);
        free(log->level);
    }
    if (log->time) {
        if (log->time->str) free(log->time->str);
        field_hl(log->time->hl);
        free(log->time);
    }
    if (log->logidstr) free(log->logidstr);
    field_hl(log->lhl);

    if (log->host) {
        if (log->host->ip) free(log->host->ip);
        field_hl(log->host->hl);
        free(log->host);
    }
    if (log->file) free(log->file);
    field_hl(log->fhl);
    if (log->extra) free(log->extra);
    field_hl(log->ehl);

    if (log->value) field_free(log->value);
}

int parse_field(Log_field *field, char *tmp) {
    int valtype = guessType(tmp);

    switch (valtype) {
        case TYPE_LONG:
            LF_LONG(field, tmp);
            break;
        case TYPE_DOUBLE:
            LF_DOUBLE(field, tmp);
            break;
        case TYPE_JSON:
            field->valjson = cJSON_ParseWithOpts(tmp, 0, 0, field->key);
            if (field->valjson == 0) {
                valtype = TYPE_STRING;
            }
            LF_STRING(field, tmp);
            break;
        case TYPE_IP:
            valtype = TYPE_STRING;
            LF_STRING(field, tmp);
            break;
        case TYPE_STRING:
            LF_STRING(field, tmp);
            break;
        default:
            valtype = TYPE_NULL;
            if(tmp) free(tmp);
            break;
    }
    L_SET_TYPE(field, valtype);
    if (valtype != TYPE_NULL) {
        L_INIT_HIGHLIGHT(field->hl);
    }

    return valtype;
}
/**
 * 格式化日志
 * @param line
 * @param lineno
 * @param failOutput
 */
void format(const char *line, const unsigned long lineno, const bool failOutput) {
    int colCnt = 0;
    Log log;

    L_INIT_LOG(&log);

    // 根据配置的格式插件，处理日志格式
    // 只要有一个处理成功了，就跳出
    for (int i = 0; i < formaterlen; ++i) {
        colCnt = formaters[i].procerfunc(&log, line, lineno);
        // 处理成功后跳出循环
        if (colCnt != FORMATER_FAILED) break;
    }

    if (FORMATER_FAILED == colCnt) {
        if (failOutput) printf("%s", line);
    } else
        print_log(&log, output_type, output_option);

    log_free(&log);
}

unsigned char cov_level_str(char *str) {
    if (!strcmp(str, LEVEL_STR_DEBUG)) return LEVEL_DEBUG;
    if (!strcmp(str, LEVEL_STR_TRACE)) return LEVEL_TRACE;
    if (!strcmp(str, LEVEL_STR_NOTICE)) return LEVEL_NOTICE;
    if (!strcmp(str, LEVEL_STR_WARNING)) return LEVEL_WARNING;
    if (!strcmp(str, LEVEL_STR_ERROR)) return LEVEL_ERROR;
    return LEVEL_UNKNOwN;
}

/**
 * 格式化插件构造
 */
void format_init(void) {
    formaterlen = sizeof(formaters) / sizeof(Formater);

    for (int i = 0; i < formaterlen; ++i)
        formaters[i].initfunc();
}
/**
 * 格式化插件析构
 */
void format_free(void) {
    for (int i = 0; i < formaterlen; ++i)
        formaters[i].destoryFunc();
}