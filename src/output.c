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
                if (json->string) P_JKEY(json->string);
                P_JSTR(json->valuestring);
                break;
            case cJSON_Number:
                if (json->string) P_JKEY(json->string);
                if (json->valuedouble == json->valueint) hl_jint(json->valueint);
                else hl_jdbl(json->valuedouble);
                break;
            case cJSON_False:
                if (json->string) hl_jkey(json->string);
                hl_jbln(json->valueint ? "true" : "false");
                break;
            case cJSON_True:
                if (json->string) hl_jkey(json->string);
                hl_jbln(json->valueint ? "true" : "false");
                break;
            case cJSON_NULL:
                if (json->string) hl_jkey(json->string);
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
    int count = 0, isPrinted = 0, lastPrinted = 0;
    Log_field *field = log->value;

    if (F_FAIL == filter_log(log)) return 0;

    if(log->host && log->host->ip && IS_FC_FAIL(COL_HOST)) {
        P_STR(COL_HOST, log->host->ip);
        count++;
    }
    if(log->level && log->level->lstr && IS_FC_FAIL(COL_LEVEL)) {
        P_STR(COL_LEVEL, log->level->lstr);
        count++;
    }

    if (IS_FC_FAIL(COL_LOGID)) {
        P_LONG(COL_LOGID, log->logid);
        count++;
    }

    if (log->file && IS_FC_FAIL(COL_FILE)) {
        P_STR(COL_FILE, log->file);
        count++;
    }
    if (log->time && log->time->str && IS_FC_FAIL(COL_TIME)) {
        P_STR(COL_TIME, log->time->str);
        count++;
    }

    while(field) {
        isPrinted = IS_FC_FAIL(field->key);
        if (isPrinted) {
            if (lastPrinted) printf(LOGR_SPC);

            print_field(field);
            count++;
            lastPrinted = isPrinted;
        }
        field = field->next;

    };

    if (log->extra && IS_FC_FAIL(COL_EXTRA)) {
        if (lastPrinted) printf(LOGR_SPC);
        hl_key(COL_EXTRA);
        hl_op(LOGR_OP);
        hl_str(log->extra);
        count++;
    }

    if (count) printf("\n");

    return 1;
}