//
// Created by Administrator on 2019/4/3.
//

#include "output/string.h"
#include "logr.h"
#include "filter.h"
#include "highlight.h"

/**
 * 字符串缓存
 */
static OutputBuffer str_buffer;


void print_log_highlight(OutputBuffer *__output, const char *key, const char *val, const Highlight *hl, const int opt) {
    if (HAS_HIGHLIGHT(hl)) {
        OT_BUF_INIT(&str_buffer);
        sprtf_hl(&str_buffer, val, hl);
        P_STR(__output, key, str_buffer.outputstr, opt);
    } else
        P_STR(__output, key, val, opt);
}

/**
 * 输出JSON至缓存
 * TODO 支持JSON高亮
 * @param __output
 * @param json
 */
void print_json_to_str(OutputBuffer *__output, cJSON *json) {
    if (!json) return;
    char *buffer = cJSON_PrintBuffered(json, (int)PRINTF_LENGTH(__output), 0);
    if (buffer) {
        P_STR_BUF(__output, buffer);
        free(buffer);
    }
}

/**
 * 打印一个字段
 *
 * @param __output
 * @param field
 */
void print_str_field(OutputBuffer *__output, const Log_field *field, const int opt) {
    // 先将所有字符打印至一个缓存中
    OT_BUF_INIT(&str_buffer);

    switch (field->type) {
        case TYPE_LONG:
        case TYPE_DOUBLE:
        case TYPE_STRING:
        case TYPE_IP:
            if (HAS_HIGHLIGHT(field->hl)) {
                sprtf_hl(&str_buffer, field->valstr->valstring, field->hl);
            } else {
                P_STR_BUF(&str_buffer, field->valstr->valstring);
            }
            break;
        case TYPE_JSON:
            if (HAS_HIGHLIGHT(field->hl)) {
                sprtf_hl(&str_buffer, field->valstr->valstring, field->hl);
            } else {
                print_json_to_str(&str_buffer, field->valjson);
            }
            break;
        default:
            return;
    }

    P_STR(__output, field->key, str_buffer.outputstr, opt);
}

int print_log_to_str_column(void *arg, const Log *log, const Column_list *col, const int opt) {
    OutputBuffer *__output = (OutputBuffer *)arg;
    int count = 0, isPrinted = 0;
    Log_field *field = log->value;

    if(log->host && log->host->ip && F_SUCC == filter_column(col, COL_HOST, FCF_TEXT)) {
        print_log_highlight(__output, COL_HOST, log->host->ip, log->host->hl, opt);
        count++;
    }

    if(log->level && log->level->lstr && F_SUCC == filter_column(col, COL_LEVEL, FCF_TEXT)) {
        print_log_highlight(__output, COL_LEVEL, log->level->lstr, log->level->hl, opt);
        count++;
    }

    if (F_SUCC == filter_column(col, COL_LOGID, FCF_TEXT)) {
        print_log_highlight(__output, COL_LOGID, log->logidstr, log->lhl, opt);
        count++;
    }

    if (log->time && log->time->valstring && F_SUCC == filter_column(col, COL_TIME, FCF_TEXT)) {
        print_log_highlight(__output, COL_TIME, log->time->valstring, NULL, opt);
        count++;
    }

    if (log->file && F_SUCC == filter_column(col, COL_FILE, FCF_TEXT)) {
        print_log_highlight(__output, COL_FILE, log->file, log->fhl, opt);
        count++;
    }

    Log_field *field1;
    for(; field; field = field->next) {
        if ((field1 = filter_fieldcolumn(col, field))) {
            print_str_field(__output, field1, opt);
            if (field1 != field) field_free(field1);
            count++;
        }
    };

    if (log->extra && F_SUCC == filter_column(col, COL_EXTRA, FCF_TEXT)) {
        P_STR(__output, COL_EXTRA, log->extra, opt);
        count++;
    }

    return count;
}

int print_log_to_str(OutputBuffer *__output, const Log *log, const int opt) {
    int count = 0;

    count = filter_column_callback(__output, log, opt, print_log_to_str_column);

    // 去掉末尾的空格
    if (is_spc(__output->outputstr - 1)) {
        *(__output->outputstr - 1) = '\0';
    }

    return count;
}