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

OutputBuffer outputBuffer;

/**
 * @param log
 * @return 已输出的字段数量
 */
int print_log(const Log *log, const int type, const int opt) {
    int count = 0;

    OT_BUF_INIT(&outputBuffer);

    if (dev_null_output && !debug_flag) return count;

    if (F_FAIL != filter_log(log)) {
        if (type == OUTPUT_STRING)
            count = print_log_to_str(&outputBuffer, log, opt);
        else if (type == OUTPUT_JSON)
            count = print_log_to_json(&outputBuffer, log, 1);
        else if (type == OUTPUT_JSON_NOREC)
            count = print_log_to_json(&outputBuffer, log, 0);

        if (0 < count && (!dev_null_output || debug_flag)) {
            fputs(outputBuffer.outputstr, stdout);
            fputs("\n", stdout);
            if (color_option)
                fputs("\r", stdout);
            fflush(stdout);
        }
    }

    return count;
}