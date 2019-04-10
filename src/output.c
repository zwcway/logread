//
// Created by Administrator on 2019/3/16.
//

#include <stdio.h>
#include "output.h"
#include "format.h"
#include "logr.h"
#include "highlight.h"
#include "filter.h"
#include "output/string.h"
#include "output/json.h"

char outputstr[MAX_LINE];

/**
 * @param log
 * @return 已输出的字段数量
 */
int print_log(const Log *log, int type) {
    char *__output = (char *)outputstr;
    int count = 0;

    if (dev_null_output) return count;

    if (F_FAIL != filter_log(log)) {
        if (type == OUTPUT_STRING)
            count = print_log_to_str(&__output, log);
        else if (type == OUTPUT_JSON)
            count = print_log_to_json(&__output, log, 1);
        else if (type == OUTPUT_JSON_NOREC)
            count = print_log_to_json(&__output, log, 0);

        if (0 < count) {
            fputs(outputstr, stdout);
            fputs("\n", stdout);
            if (color_option)
                fputs("\r", stdout);
            fflush(stdout);
        }
    }

    return count;
}