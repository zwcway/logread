//
// Created by Administrator on 2019/3/16.
//

#include <stdio.h>
#include "output.h"
#include "format.h"
#include "logr.h"
#include "highlight.h"
#include "filter.h"

void print_json(cJSON *json) {
    cJSON *next;
    if (!json) return;

    while(json) {
        next = json->next;

        switch (json->type) {
            case cJSON_String:
                P_JKEY(json->string);
                P_JSTR(json->valuestring);
                break;
            case cJSON_Number:
                P_JKEY(json->string);
                if (json->valuedouble == json->valueint) hl_jint(json->valueint);
                else hl_jdbl(json->valuedouble);
                break;
            case cJSON_False:
                hl_jkey(json->string);
                hl_jbln(json->valueint ? "true" : "false");
                break;
            case cJSON_True:
                hl_jkey(json->string);
                hl_jbln(json->valueint ? "true" : "false");
                break;
            case cJSON_NULL:
                hl_jkey(json->string);
                hl_jnul("NULL");
                break;
            case cJSON_Object:
                if (json->string) P_JKEY(json->string);
                hl_jobj("{");
                print_json(json->child);
                hl_jobj("}");
                break;
            case cJSON_Array:
                hl_jary("[");
                print_json(json->child);
                hl_jary("]");
                break;
            default:
                break;
        }

        if (next) hl_jcma(",");

        json = next;
    }
}

void print_field(const Log_field *field) {
    hl_key(field->key);
    hl_op(LOGR_OP);

    switch (field->type) {
        case TYPE_STRING:
        case TYPE_IP:
            hl_str(field->val.valstr->valstring);
            break;
        case TYPE_LONG:
            hl_long(field->val.valstr->vallong);
            break;
        case TYPE_DOUBLE:
            hl_double(field->val.valstr->valdbl);
            break;
        case TYPE_JSON:
            print_json(field->val.valjson);
            break;
        default:break;
    }
}
/**
 * TODO 输出json字符串
 * @param log
 * @return
 */
int print_log(const Log *log) {
    Log_field *field = log->value;

    if (F_FAIL == filter_log(log)) return 0;

    if(log->host && log->host->ip) P_STR(COL_HOST, log->host->ip);
    if(log->level && log->level->lstr) P_STR(COL_LEVEL, log->level->lstr);

    P_LONG(COL_LOGID, log->logid);

    if (log->file) P_STR(COL_FILE, log->file);
    if (log->time && log->time->str) P_STR(COL_TIME, log->time->str);

    while(field) {
        print_field(field);

        field = field->next;

        if(field) printf(LOGR_SPC);
    };

    if (log->extra) {
        printf(LOGR_SPC); hl_key(COL_EXTRA); hl_op(LOGR_OP); hl_str(log->extra);
    }

    printf("\n");

    return 1;
}