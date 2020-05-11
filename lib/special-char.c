//
// Created by Administrator on 2020/5/11.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "special-char.h"

/**
 * 转义字符串
 * @param input
 * @param output
 * @return
 */
int convert_string(char * input)
{
    int length = 0;
    char chr;
    char *output = input;

    while ('\0' != (chr = *(input ++))) {
        if (chr == '\\') {
            switch (*input) {
                case 'n':*output++ = '\n';break;
                case 'r':*output++ = '\r';break;
                case 't':*output++ = '\t';break;
                default:
                    *output++ = '\\';
                    *output++ = *input;
                    break;
            }
            input++;
        } else {
            *output++ = chr;
        }
    }
    *output = '\0';

    return length;
}
