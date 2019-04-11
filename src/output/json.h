//
// Created by Administrator on 2019/4/3.
//

#ifndef LOGR_JSON_H
#define LOGR_JSON_H

#include "format.h"
#include "cJSON.h"
#include "output.h"

extern int print_log_to_json(OutputBuffer*, const Log *, int);

#endif //LOGR_JSON_H
