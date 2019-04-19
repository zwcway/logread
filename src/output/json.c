//
// Created by Administrator on 2019/4/3.
//

#include "logr.h"
#include "format.h"
#include "filter.h"
#include "cJSON.h"
#include "output/string.h"

void print_json_field(cJSON *__json, const Log_field *field, const int _rec) {
    cJSON * _item = 0;

    switch (field->type) {
        case TYPE_STRING:
        case TYPE_IP:
            _item = cJSON_CreateString(field->valstr->valstring ? field->valstr->valstring : "");
            break;
        case TYPE_LONG:
        case TYPE_DOUBLE:
            _item = cJSON_CreateNumber(field->valstr->valdbl);
            break;
        case TYPE_JSON:
            if (_rec) _item = cJSON_Duplicate(field->valjson, 1);
            else _item = cJSON_CreateString(field->valstr->valstring);
            break;
        default:break;
    }

    cJSON_AddItemToObject(__json, field->key, _item);
}

int print_log_to_json_column(void *arg, const Log *log, const Column_list *col, const int opt) {
    cJSON *__json = (cJSON *)arg;
    int count = 0;
    Log_field *field = log->value;
    cJSON * _item;

    if(log->host && log->host->ip && F_SUCC == filter_column(col, COL_HOST)) {
        _item = cJSON_CreateString(log->host->ip);
        cJSON_AddItemToObject(__json, COL_HOST, _item);
        count++;
    }

    if(log->level && log->level->lstr && F_SUCC == filter_column(col, COL_LEVEL)) {
        _item = cJSON_CreateString(log->level->lstr);
        cJSON_AddItemToObject(__json, COL_LEVEL, _item);
        count++;
    }

    if (F_SUCC == filter_column(col, COL_LOGID)) {
        _item = cJSON_CreateNumber((double)log->logid);
        cJSON_AddItemToObject(__json, COL_LOGID, _item);
        count++;
    }

    if (log->file && F_SUCC == filter_column(col, COL_FILE)) {
        _item = cJSON_CreateString(log->file);
        cJSON_AddItemToObject(__json, COL_FILE, _item);
        count++;
    }
    if (log->time && log->time->str && F_SUCC == filter_column(col, COL_TIME)) {
        _item = cJSON_CreateString(log->time->str);
        cJSON_AddItemToObject(__json, COL_TIME, _item);
        count++;
    }

    Log_field *field1;
    for(; field; field = field->next) {
        if ((field1 = filter_fieldcolumn(col, field))) {
            print_json_field(__json, field1, opt);
            if (field1 != field) field_free(field1);
            count++;
        }
    };

    if (log->extra && F_SUCC == filter_column(col, COL_EXTRA)) {
        _item = cJSON_CreateString(log->extra);
        cJSON_AddItemToObject(__json, COL_EXTRA, _item);
        count++;
    }

    return count;
}

/**
 *
 * @param __str
 * @param log
 * @param _rec 是否递归处理子json
 * @return
 */
int print_log_to_json(OutputBuffer*__str, const Log *log, const int _rec) {
    cJSON *__json = cJSON_CreateObject();
    int opt = 0;

    if (_rec) opt |= FC_OPT_RECURSE;

    int count = filter_column_callback(__json, log, opt, print_log_to_json_column);

    print_json_to_str(__str, __json);

    cJSON_Delete(__json);

    return count;
}