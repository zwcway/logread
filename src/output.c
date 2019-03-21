//
// Created by Administrator on 2019/3/16.
//

#include <stdio.h>
#include "output.h"
#include "format.h"
#include "logr.h"
#include "highlight.h"

void print_json(cJSON *json) {

}

void print_field(const Log_field *field) {
    hl_key(field->key);
    hl_op(LOGR_OP);

    switch (field->type) {
        case TYPE_STRING:
            hl_str(field->val->valstring);
            break;
        case TYPE_INT:
//            hl_int(field->val->vallong);
            break;
        case TYPE_JSON:
//            hl_int(field->val->valjson);
            break;
    }
}

void print_log(const Log *log) {
    Log_field *field = log->value;

    P_STR(COL_LEVEL, cov_level_int(log->level));
    P_INT(COL_LOGID, log->logid);

    if (log->file) P_STR(COL_FILE, log->file);
    if (log->time) P_STR(COL_TIME, log->time);

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