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
            _item = cJSON_CreateString(field->valstr->valstring);
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

int print_log_json(cJSON *__json, const Log *log, const int _rec) {
    int count = 0;
    Log_field *field = log->value;
    cJSON * _item;

    if(log->host && log->host->ip && IS_FC_FAIL(COL_HOST)) {
        _item = cJSON_CreateString(log->host->ip);
        cJSON_AddItemToObject(__json, COL_HOST, _item);
        count++;
    }

    if(log->level && log->level->lstr && IS_FC_FAIL(COL_LEVEL)) {
        _item = cJSON_CreateString(log->level->lstr);
        cJSON_AddItemToObject(__json, COL_LEVEL, _item);
        count++;
    }

    if (IS_FC_FAIL(COL_LOGID)) {
        _item = cJSON_CreateNumber((double)log->logid);
        cJSON_AddItemToObject(__json, COL_LOGID, _item);
        count++;
    }

    if (log->file && IS_FC_FAIL(COL_FILE)) {
        _item = cJSON_CreateString(log->file);
        cJSON_AddItemToObject(__json, COL_FILE, _item);
        count++;
    }
    if (log->time && log->time->str && IS_FC_FAIL(COL_TIME)) {
        _item = cJSON_CreateString(log->time->str);
        cJSON_AddItemToObject(__json, COL_TIME, _item);
        count++;
    }

    while(field) {
        if (IS_FC_FAIL(field->key)) {
            print_json_field(__json, field, _rec);
            count++;
        }
        field = field->next;
    };

    if (log->extra && IS_FC_FAIL(COL_EXTRA)) {
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
int print_log_to_json(char **__str, const Log *log, const int _rec) {
    cJSON *__json = cJSON_CreateObject();

    int count = print_log_json(__json, log, _rec);

    print_json_to_str(__str, __json);

    cJSON_Delete(__json);

    return count;
}