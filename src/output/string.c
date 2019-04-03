//
// Created by Administrator on 2019/4/3.
//

#include "output/string.h"
#include "logr.h"
#include "filter.h"

void print_json_to_str(char **__output, cJSON *json) {
    cJSON *next;
    if (!json) return;

    while(json) {
        next = json->next;

        switch (json->type) {
            case cJSON_String:
                if (json->string) P_JKEY(__output, json->string);
                P_JSTR(__output, json->valuestring);
                break;
            case cJSON_Number:
                if (json->string) P_JKEY(__output, json->string);
                if (json->valuedouble == (double)json->valueint) hl_jint(__output, json->valueint);
                else hl_jdbl(__output, json->valuedouble);
                break;
            case cJSON_False:
                if (json->string) hl_jkey(__output, json->string);
                hl_jbln(__output, json->valueint ? "true" : "false");
                break;
            case cJSON_True:
                if (json->string) hl_jkey(__output, json->string);
                hl_jbln(__output, json->valueint ? "true" : "false");
                break;
            case cJSON_NULL:
                if (json->string) hl_jkey(__output, json->string);
                hl_jnul(__output, "NULL");
                break;
            case cJSON_Object:
                if (json->string) P_JKEY(__output, json->string);
                hl_jobj(__output, "{");
                print_json_to_str(__output, json->child);
                hl_jobj(__output, "}");
                break;
            case cJSON_Array:
                hl_jary(__output, "[");
                print_json_to_str(__output, json->child);
                hl_jary(__output, "]");
                break;
            default:
                break;
        }

        if (next) hl_jcma(__output, ",");

        json = next;
    }
}

void print_str_field(char **__output, const Log_field *field) {
    hl_key(__output, field->key);
    hl_op(__output, LOGR_OP);

    switch (field->type) {
        case TYPE_STRING:
        case TYPE_IP:
            hl_str(__output, field->valstr->valstring);
            break;
        case TYPE_LONG:
            hl_long(__output, field->valstr->vallong);
            break;
        case TYPE_DOUBLE:
            hl_double(__output, field->valstr->valdbl);
            break;
        case TYPE_JSON:
            print_json_to_str(__output, field->valjson);
            break;
        default:break;
    }
}

int print_log_to_str(char **__output, const Log *log) {
    int count = 0, isPrinted = 0, lastPrinted = 0;
    Log_field *field = log->value;

    if(log->host && log->host->ip && IS_FC_FAIL(COL_HOST)) {
        P_STR(__output, COL_HOST, log->host->ip);
        count++;
    }

    if(log->level && log->level->lstr && IS_FC_FAIL(COL_LEVEL)) {
        P_STR(__output, COL_LEVEL, log->level->lstr);
        count++;
    }

    if (IS_FC_FAIL(COL_LOGID)) {
        P_LONG(__output, COL_LOGID, log->logid);
        count++;
    }

    if (log->file && IS_FC_FAIL(COL_FILE)) {
        P_STR(__output, COL_FILE, log->file);
        count++;
    }
    if (log->time && log->time->str && IS_FC_FAIL(COL_TIME)) {
        P_STR(__output, COL_TIME, log->time->str);
        count++;
    }

    while(field) {
        isPrinted = IS_FC_FAIL(field->key);
        if (isPrinted) {
            if (lastPrinted) P_SPC(__output, LOGR_SPC);

            print_str_field(__output, field);
            count++;
            lastPrinted = isPrinted;
        }
        field = field->next;

    };

    if (log->extra && IS_FC_FAIL(COL_EXTRA)) {
        if (lastPrinted) P_SPC(__output, LOGR_SPC);
        hl_key(__output, COL_EXTRA);
        hl_op(__output, LOGR_OP);
        hl_str(__output, log->extra);
        count++;
    }

    return count;
}