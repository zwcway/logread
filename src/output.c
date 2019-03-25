//
// Created by Administrator on 2019/3/16.
//

#include <stdio.h>
#include "output.h"
#include "format.h"
#include "logr.h"
#include "highlight.h"

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
                hl_jstr(json->valueint ? "true" : "false");
                break;
            case cJSON_Object:
                if (json->string) P_JKEY(json->string);

                hl_jstr("{");
                print_json(json->child);
                hl_jstr("}");
                break;
            case cJSON_Array:
                hl_jstr("[");
                print_json(json->child);
                hl_jstr("]");
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
            hl_str(field->val->valstring);
            break;
        case TYPE_LONG:
            hl_long(field->val->vallong);
            break;
        case TYPE_DOUBLE:
            hl_double(field->val->valdbl);
            break;
        case TYPE_JSON:
            print_json(field->val->valjson);
            break;
    }
}

void print_log(const Log *log) {
    Log_field *field = log->value;

    if(log->host.ip) P_STR(COL_HOST, log->host.ip);
    if(log->level.lstr) P_STR(COL_LEVEL, log->level.lstr);

    P_LONG(COL_LOGID, log->logid);

    if (log->file) P_STR(COL_FILE, log->file);
    if (log->time.str) P_STR(COL_TIME, log->time.str);

    while(field) {
        print_field(field);

        field = field->next;

        if(field) printf(LOGR_SPC);
    };

    if (log->extra) {
        printf(LOGR_SPC); hl_key(COL_EXTRA); hl_op(LOGR_OP); hl_str(log->extra);
    }

    printf("\n");
}