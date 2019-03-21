//
// Created by Administrator on 2019/3/19.
//
#include <ctype.h>

int isInteger(char *str) {
    int j = 0, len = 0;
    while (*str != '\0') //遍历整个字符串
    {
        if (*str <= 57 && *str >= 48) //0~9的ASCII码是48~57
            j++; //找到数字了就数量++

        len++;
        str++;
    }

    //数字总数和字符串长度一样，则全是数字，总数为0，则都不是数字，在0~len之间则有部分是数字
    if (j == len) return 1;

    return 0;
}

int guessType(char *str) {
    int numcnt = 0, poicnt = 0, len = 0;
    int isNumber = 0, isInteger = 0, isJson = 0;

    //遍历整个字符串
    while (*str != '\0') {
        //0~9的ASCII码是48~57
        if (*str <= 57 && *str >= 48) {
            numcnt++;
        } else if (*str == '.') {
            poicnt++;
        } else if (*str == '-' || *str == '+') {
            if (len == 0) {
                isNumber = 1;
                isInteger = 1;
            }
        } else {
            isNumber = 0;
            isInteger = 0;
        }

        len++;
        str++;
    }

    if (poicnt > 1) isNumber = 0;
    if (poicnt > 0) isInteger = 0;

    if (numcnt == len) isInteger = 1;

    return 0;
}