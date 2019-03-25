//
// Created by Administrator on 2019/3/25.
//


#include "lstring.h"

int strprintf(const String str) {
    return printf("%.*s", str.len, str.str);
}

