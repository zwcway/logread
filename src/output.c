//
// Created by Administrator on 2019/3/16.
//

#include <stdio.h>
#include "format.h"
#include "logr.h"

void print_json(cJSON *json) {

}

void print_log(const Log *log) {
    Log_field *field = log->value;

    printf("%s=%s ", COL_LEVEL, cov_level_int(log->level));
    if (log->file) printf("%s=%s ", COL_FILE, log->file);
    printf("%s=%d ", COL_LOGID, log->logid);
    if (log->time) printf("%s=%s ", COL_TIME, log->time);

    while(field) {
        printf("%s=", field->key);
        switch (field->type) {
            case TYPE_STRING:
                printf("%s", field->val->valstring);
                break;
            case TYPE_INT:
                printf("%d", (int)field->val->vallong);
                break;
            case TYPE_JSON:
                print_json(field->val->valjson);
                break;
        }
        field = field->next;

        if(field) printf(" ");
    };

    if (log->extra) printf("%s=%s", COL_EXTRA, log->extra);

    printf("\n");
}