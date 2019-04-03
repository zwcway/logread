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
 * TODO 输出json字符串
 * @param log
 * @return 已输出的字段数量
 */
int print_log(const Log *log, int type) {
    char *__output = (char *)outputstr;
    int count = 0;

    if (F_FAIL != filter_log(log)) {
        if (type == OUTPUT_STRING)
            count = print_log_to_str(&__output, log);
        else if (type == OUTPUT_JSON)
            count = print_log_to_json(&__output, log, 1);
        else if (type == OUTPUT_JSON_NOREC)
            count = print_log_to_json(&__output, log, 0);

        if (0 < count)
            printf("%s\n", outputstr);
    }

    return count;
}