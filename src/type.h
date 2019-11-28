//
// Created by Administrator on 2019/3/22.
//

#ifndef LOGR_TYPE_H
#define LOGR_TYPE_H

#include <ctype.h>
#include <time.h>
#include <arpa/inet.h>
#include "type/time.h"


#define TYPE_STRING 1
#define TYPE_LONG 2
#define TYPE_DOUBLE 3
#define TYPE_JSON 4
#define TYPE_IP 5
#define TYPE_URL 6
#define TYPE_TIME 7
#define TYPE_NULL 9

//int isNumber(char *str) { return isdigit(str); };

int isInteger(const char *str);
int isJSON(const char *str);
int isIpV4(const char *ipAddress);
int guessType(const char *str, Time *pTime);

#endif //LOGR_TYPE_H
