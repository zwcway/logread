//
// Created by Administrator on 2019/3/13.
//

#ifndef LOGR_FORMAT_H
#define LOGR_FORMAT_H

#include "cJSON.h"
#include "string.h"


#define LEVEL_DEBUG 1
#define LEVEL_TRACE 2
#define LEVEL_NOTICE 3
#define LEVEL_WARNING 4
#define LEVEL_ERROR 5
#define LEVEL_UNKNOwN 0

#define LEVEL_STR_DEBUG "DEBUG"
#define LEVEL_STR_TRACE "TRACE"
#define LEVEL_STR_NOTICE "NOTICE"
#define LEVEL_STR_WARNING "WARNING"
#define LEVEL_STR_ERROR  "ERROR"
#define LEVEL_STR_UNKNOWN  "UNKNOWN"

static unsigned char cov_level_str(char *str) {
    if (!strcmp(str, LEVEL_STR_DEBUG)) return LEVEL_DEBUG;
    if (!strcmp(str, LEVEL_STR_TRACE)) return LEVEL_TRACE;
    if (!strcmp(str, LEVEL_STR_NOTICE)) return LEVEL_NOTICE;
    if (!strcmp(str, LEVEL_STR_WARNING)) return LEVEL_WARNING;
    if (!strcmp(str, LEVEL_STR_ERROR)) return LEVEL_ERROR;
    return LEVEL_UNKNOwN;
}

static char *cov_level_int(unsigned char level) {
    switch (level) {
        case LEVEL_DEBUG:return LEVEL_STR_DEBUG;
        case LEVEL_TRACE:return LEVEL_STR_TRACE;
        case LEVEL_NOTICE:return LEVEL_STR_NOTICE;
        case LEVEL_WARNING:return LEVEL_STR_WARNING;
        case LEVEL_ERROR:return LEVEL_STR_ERROR;
        default:return LEVEL_STR_UNKNOWN;
    }
}

#define TYPE_STRING 1
#define TYPE_INT 2
#define TYPE_JSON 3


#define OP_OPEN '['
#define OP_CLOSE ']'
#define OP_SPER  ' '


/**
 *
 */
typedef union Log_value {
    size_t vallong;
    char *valstring;
    cJSON *valjson;
} Log_value;

/**
 * 字段值
 */
typedef struct Log_field {
    char *key;
    unsigned char type;
    Log_value *val;
    struct Log_field *next, *prev;
} Log_field;

/**
 * 日志
 */
typedef struct Log {
    unsigned char level;
    /** 生成时间 */
    char *time;
    int ts;
    /** 调用文件 */
    char *file;
    /** 日志id */
    int logid;
    /** 其他扩展字符串 */
    char *extra;
    /** 链表 */
    Log_field *value;
} Log;

#define L_VAL(log) ((Log)(log)).value

#define L_SET_LEVEL(log, l)   (log).level = (unsigned char)l
#define L_SET_WARNING(log)    L_SETLEVEL(log, LEVEL_WARNING)
#define L_SET_ERROR(log)      L_SETLEVEL(log, LEVEL_ERROR)
#define L_SET_NOTICE(log)     L_SETLEVEL(log, LEVEL_NOTICE)
#define L_SET_DEBUG(log)      L_SETLEVEL(log, LEVEL_DEBUG)

#define L_SET_TYPE(field, t)    ((Log_field*)(field))->type = (unsigned char)t
#define L_SET_STRING(field)     L_SET_TYPE(field, TYPE_STRING)
#define L_SET_INT(field)        L_SET_TYPE(field, TYPE_INT)
#define L_SET_JSON(field)       L_SET_TYPE(field, TYPE_JSON)


#define L_INIT_FIELD(field)    do { \
field = (Log_field *)malloc(sizeof(Log_field)); \
field->key = 0; \
field->next = 0; \
field->prev = 0; \
field->val = (Log_value *)malloc(sizeof(Log_value)); \
field->val->vallong = 0; \
field->val->valstring = 0; \
field->val->valjson = 0; \
}while(0)

#define L_FIELD_SKEY(field, key)    do { \
field->key = key; \
}while(0)

#define L_ADD_FIELD(field)    do { \
Log_field *add; \
L_INIT_FIELD(add); \
field->next = add; \
add->prev = field; \
field = field->next; \
}while(0)


void format(const char *log);
static char* sub_str_trim(const char *str, size_t len, unsigned char trim);

#define sub_str(str, len)  sub_str_trim(str, len, 0)
#define sub_trim(str, len)  sub_str_trim(str, len, 1)

#endif //LOGR_FORMAT_H
