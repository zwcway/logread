//
// Created by Administrator on 2019/3/19.
//
#include "type.h"


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

int conv2IpV4(char *ipAddress, struct in_addr *addr) {
    int result = inet_pton(AF_INET, ipAddress, addr);
    return result != 0;
}

int isIpV4(char *ipAddress) {
    struct in_addr sa;
    return conv2IpV4(ipAddress, &sa);
}

int guessType(char *str) {
    int numcnt = 0, poicnt = 0, len = 0;
    int isNumber = 0, isInteger = 0;
    char firstchr, lastchr;

    if (str == 0 || *str == '\0') return TYPE_NULL;

    firstchr = *str;

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

    lastchr = *(str - 1);

    if ((firstchr == '{' && lastchr == '}') || (firstchr == '[' && lastchr == ']'))
        return TYPE_JSON;

    if (poicnt > 1) isNumber = 0;
    if (poicnt > 0) isInteger = 0;

    if (numcnt == len) isInteger = 1;

    if (isInteger) return TYPE_LONG;
    if (isNumber) return TYPE_DOUBLE;

    if (isIpV4(str)) return TYPE_IP;

    return TYPE_STRING;
}