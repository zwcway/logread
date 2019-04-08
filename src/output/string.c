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
static char str_buffer[MAX_LINE];

/**
 * 输出JSON至缓存
 * TODO 支持JSON高亮
 * TODO 不使用递归
 * @param __output
 * @param json
 */
void print_json_to_str(char **__output, cJSON *json) {
    if (!json) return;

    for(; json ; json = json->next) {
        switch (json->type) {
            case cJSON_String:
                if (json->string) P_STRQJ_BUF(__output, json->string);
                P_STRQ_BUF(__output, json->valuestring);
                break;
            case cJSON_Number:
                if (json->string) P_STRQJ_BUF(__output, json->string);
                P_STR_BUF(__output, json->valuestring);
                break;
            case cJSON_False:
            case cJSON_True:
                if (json->string) P_STRQJ_BUF(__output, json->string);
                P_STR_BUF(__output, json->valueint ? "true" : "false");
                break;
            case cJSON_NULL:
                if (json->string) P_STRQJ_BUF(__output, json->string);
                P_STR_BUF(__output, "NULL");
                break;
            case cJSON_Object:
                if (json->string) P_STRQJ_BUF(__output, json->string);
                P_STR_BUF(__output, "{");
                print_json_to_str(__output, json->child);
                P_STR_BUF(__output, "}");
                break;
            case cJSON_Array:
                P_STR_BUF(__output, "[");
                print_json_to_str(__output, json->child);
                P_STR_BUF(__output, "]");
                break;
            default:
                break;
        }

        if (json->next) P_STR_BUF(__output, ",");
    }
}
/**
 * 打印一个字段
 *
 * @param __output
 * @param field
 */
void print_str_field(char **__output, const Log_field *field) {
    // 先将所有字符打印至一个缓存中
    char *val = str_buffer;

    switch (field->type) {
        case TYPE_LONG:
        case TYPE_DOUBLE:
        case TYPE_STRING:
        case TYPE_IP:
            P_STR_BUF(&val, field->valstr->valstring);
            break;
        case TYPE_JSON:
            print_json_to_str(&val, field->valjson);
            break;
        default:break;
    }

    P_STR(__output, field->key, str_buffer);
}

int print_log_to_str_column(void *arg, const Log *log, const Column_list *col, const int opt) {
    char **__output = (char **)arg;
    int count = 0, isPrinted = 0;
    Log_field *field = log->value;

    if(log->host && log->host->ip && F_FAIL == filter_column(col, COL_HOST)) {
        P_STR(__output, COL_HOST, log->host->ip);
        count++;
    }

    if(log->level && log->level->lstr && F_FAIL == filter_column(col, COL_LEVEL)) {
        P_STR(__output, COL_LEVEL, log->level->lstr);
        count++;
    }

    if (F_FAIL == filter_column(col, COL_LOGID)) {
        P_STR(__output, COL_LOGID, log->logidstr);
        count++;
    }

    if (log->file && F_FAIL == filter_column(col, COL_FILE)) {
        P_STR(__output, COL_FILE, log->file);
        count++;
    }
    if (log->time && log->time->str && F_FAIL == filter_column(col, COL_TIME)) {
        P_STR(__output, COL_TIME, log->time->str);
        count++;
    }

    for(; field; field = field->next) {
        isPrinted = (F_FAIL == filter_column(col, field->key));
        if (isPrinted) {
            print_str_field(__output, field);
            count++;
        }
    };

    if (log->extra && F_FAIL == filter_column(col, COL_EXTRA)) {
        P_STR(__output, COL_EXTRA, log->extra);
        count++;
    }

    return count;
}

int print_log_to_str(char **__output, const Log *log) {
    int count = 0, opt = 0;

    count = filter_column_callback(__output, log, opt, print_log_to_str_column);

    // 去掉末尾的空格
    if (is_spc((*__output) - 1)) {
        *((*__output) - 1) = '\0';
    }

    return count;
}