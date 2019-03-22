//
// Created by Administrator on 2019/3/22.
//

#ifndef LOGR_TYPE_H
#define LOGR_TYPE_H

#include <ctype.h>
#include <arpa/inet.h>

//int isNumber(char *str) { return isdigit(str); };

int isInteger(char *str);
int isIpV4(char *ipAddress);

#endif //LOGR_TYPE_H
