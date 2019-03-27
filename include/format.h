//
// Created by Administrator on 2019/3/13.
//

#ifndef LOGR_FORMAT_H
#define LOGR_FORMAT_H

#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "cJSON.h"
#include "type.h"


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

#define OP_OPEN '['
#define OP_CLOSE ']'
#define OP_SPER  ' '

/**
 *
 */
typedef struct Log_value {
    double valdbl;
    long long vallong;
    char *valstring;
} Log_value;

/**
 * 字段值
 */
typedef struct Log_field {
    char *key;
    unsigned char type;
    union {
        Log_value *valstr;
        cJSON *valjson;
    } val;
    struct Log_field *next, *prev;
} Log_field;

typedef struct Log_time {
    time_t ts;
    char *str;
} Log_time;
typedef struct Log_host {
    in_addr_t lip;
    char *ip;
} Log_host;
typedef struct Log_level {
    unsigned char lint;
    char *lstr;
} Log_level;
/**
 * 日志
 */
typedef struct Log {
    unsigned long pos;
    Log_level *level;
    Log_host *host;
    /** 生成时间 */
    Log_time *time;
    /** 调用文件 */
    char *file;
    /** 日志id */
    long long logid;
    /** 其他扩展字符串 */
    char *extra;
    /** 链表 */
    Log_field *value;
    const char *line;
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

#define L_INIT_LOG(log)   do { \
log.level = 0; \
log.host = 0; \
log.time = 0; \
log.file = 0; \
log.logid = 0; \
log.extra = 0; \
log.value = 0; \
log.pos = 0; \
} while(0)

#define L_INIT_FIELD(field)    do { \
field = (Log_field *)malloc(sizeof(Log_field)); \
field->key = 0; \
field->next = 0; \
field->prev = 0; \
field->val.valstr = 0; \
}while(0)

#define L_INIT_VALUE(field)  do { \
field->val.valstr = (Log_value *)malloc(sizeof(Log_value)); \
field->val.valstr->vallong = 0; \
field->val.valstr->valdbl = 0; \
field->val.valstr->valstring = 0; \
}while(0)

#define L_INIT_HOST(log)  do { \
log.host = (Log_host *)malloc(sizeof(Log_host)); \
log.host->lip = 0; \
log.host->ip = 0; \
}while(0)

#define L_INIT_TIME(log)  do { \
log.time = (Log_time *)malloc(sizeof(Log_time)); \
log.time->ts = 0; \
log.time->str = 0; \
}while(0)

#define L_INIT_LEVEL(log)  do { \
log.level = (Log_level *)malloc(sizeof(Log_level)); \
log.level->lint = 0; \
log.level->lstr = 0; \
}while(0)

#define LF_LONG(field, tmp) do { \
L_INIT_VALUE(field); \
field->val.valstr->vallong = atol(tmp); \
field->val.valstr->valdbl = atof(tmp); \
field->val.valstr->valstring = tmp; \
}while(0)

#define LF_DOUBLE(field, tmp) do { \
L_INIT_VALUE(field); \
field->val.valstr->vallong = atol(tmp); \
field->val.valstr->valdbl = atof(tmp); \
field->val.valstr->valstring = tmp; \
}while(0)

#define LF_STRING(field, tmp) do { \
L_INIT_VALUE(field); \
field->val.valstr->valstring = tmp; \
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


void format(const char *log, const unsigned long count);

/**
 * 取子字符串，并支持删除两边空格
 *
 * TODO 使用 lstring，避免使用 memcpy
 * @param str
 * @param len
 * @param trim 0保持原样；1删除空格
 * @return
 */
static char* sub_str_trim(const char *str, size_t len, bool trim) {
    size_t reallen;
    char *copy;
    char *src = (char *)str;

    if (len <= 0) return 0;

    reallen = strlen(src);
    if (reallen < len) {
        len = reallen;
    }

    if (trim) {
        while (*src == ' ') {
            src++;
            len--;
        }
        while (*(src + len - 1) == ' ' || *(src + len - 1) == '\n' || *(src + len - 1) == '\r') len--;
    }

    len++;

    if (!(copy = (char *) malloc(len))) return 0;

    if (*(src + len - 1) != '\0') len--;

    memcpy(copy, src, len);

    if (*(src + len) != '\0') *(copy + len) = '\0';

    return copy;
}

#define sub_str(str, len)  sub_str_trim(str, len, 0)
#define sub_trim(str, len)  sub_str_trim(str, len, 1)

#endif //LOGR_FORMAT_H
