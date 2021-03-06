//
// Created by Administrator on 2019/3/13.
//

#ifndef LOGR_FORMAT_H
#define LOGR_FORMAT_H

#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <regex.h>
#include <stdbool.h>
#include "lstring.h"
#include "utils.h"
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

typedef struct _formater Formater;

extern unsigned char cov_level_str(char *str);

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
#define OP_DQOUTE  '"'
#define OP_QOUTE  '\''
#define OP_EQUAL  '='

typedef struct Highlight {
    char *pre;
    char *str;
    char *app;
} Highlight;


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
    Log_value *valstr;
    cJSON *valjson;
    struct Log_field *next, *prev;
    Highlight *hl;
} Log_field;

typedef struct Log_host {
    in_addr_t lip;
    char *ip;
    Highlight *hl;
} Log_host;
typedef struct Log_level {
    unsigned char lint;
    char *lstr;
    Highlight *hl;
} Log_level;
/**
 * 日志
 */
typedef struct Log {
    unsigned long pos;
    Log_level *level;
    Log_host *host;
    /** 生成时间 */
    Log_value *time;
    /** 调用文件 */
    char *file;
    Highlight *fhl;
    /** 日志id */
    long long logid;
    char * logidstr;
    Highlight *lhl;
    /** 其他扩展字符串 */
    char *extra;
    Highlight *ehl;
    /** 链表 */
    Log_field *value;
    const char *line;
} Log;

#define FORMATER_PROC_ARGS          Log *log, const char *log_line, unsigned long lineno
#define FORMATER_INIT_ARGS void
/**
 * 格式解析失败
 */
#define FORMATER_FAILED -1
/**
 * 格式解析失败，但是存在过滤，不输出原日志
 */
#define FORMATER_FILTED -2

typedef int (*FmtProcFunc)(FORMATER_PROC_ARGS);
typedef void (*FmtInitFunc)(FORMATER_INIT_ARGS);
typedef void (*FmtDestoryFunc)(FORMATER_INIT_ARGS);

struct _formater {
    FmtInitFunc initfunc;
    FmtProcFunc procerfunc;
    FmtDestoryFunc destoryFunc;
};


#define FORMATER_INIT_FUNCNAME(name)    init_##name
#define FORMATER_PROC_FUNCNAME(name)    format_##name
#define FORMATER_DESTORY_FUNCNAME(name)    destory_##name
#define FORMATER_INIT_FUNC(name)        void FORMATER_INIT_FUNCNAME(name)(FORMATER_INIT_ARGS)
#define FORMATER_DESTORY_FUNC(name)     void FORMATER_DESTORY_FUNCNAME(name)(FORMATER_INIT_ARGS)
#define FORMATER_PROC_FUNC(name)        int FORMATER_PROC_FUNCNAME(name)(FORMATER_PROC_ARGS)

#define ADD_FORMATER(name)              { \
FORMATER_INIT_FUNCNAME(name), \
FORMATER_PROC_FUNCNAME(name), \
FORMATER_DESTORY_FUNCNAME(name) \
}

#define L_VAL(log) ((Log)(log)).value

#define L_SET_LEVEL(log, l)   (log).level = (unsigned char)(l)
#define L_SET_WARNING(log)    L_SETLEVEL(log, LEVEL_WARNING)
#define L_SET_ERROR(log)      L_SETLEVEL(log, LEVEL_ERROR)
#define L_SET_NOTICE(log)     L_SETLEVEL(log, LEVEL_NOTICE)
#define L_SET_DEBUG(log)      L_SETLEVEL(log, LEVEL_DEBUG)

#define L_SET_TYPE(field, t)    ((Log_field*)(field))->type = (unsigned char)t
#define L_SET_STRING(field)     L_SET_TYPE(field, TYPE_STRING)
#define L_SET_INT(field)        L_SET_TYPE(field, TYPE_INT)
#define L_SET_JSON(field)       L_SET_TYPE(field, TYPE_JSON)

/**
 * 初始化字段中的高亮器结构
 */
#define L_INIT_HIGHLIGHT(__hl) do { \
if (NULL == __hl) { \
    __hl = (Highlight *)malloc(sizeof(Highlight)); \
} \
if (NULL != __hl) { \
    (__hl)->pre = NULL; \
    (__hl)->app = NULL; \
    (__hl)->str = NULL; \
} \
} while(0)

/**
 * 初始化一个字段结构
 */
#define L_INIT_FIELD(field)    do { \
(field) = (Log_field *)calloc(1, sizeof(Log_field)); \
if (NULL!=field) L_INIT_HIGHLIGHT((field)->hl); \
}while(0)

/**
 * 初始化字段中的通用值结构
 */
#define L_INIT_VALUE(field)  do { \
if(NULL!=field) (field)->valstr = (Log_value *)calloc(1, sizeof(Log_value));\
}while(0)

/**
 * 初始化字段中的域名结构
 */
#define L_INIT_HOST(log)  do { \
(log)->host = (Log_host *)calloc(1, sizeof(Log_host)); \
if (NULL != (log)->host) L_INIT_HIGHLIGHT((log)->host->hl); \
}while(0)

/**
 * 初始化字段中的时间结构
 */
#define L_INIT_TIME(log)  do { \
(log)->time = (Log_value *)calloc(1, sizeof(Log_value)); \
/* if (NULL != (log)->time) L_INIT_HIGHLIGHT((log)->time->hl);*/ \
}while(0)

/**
 * 初始化字段中的日志等级结构
 */
#define L_INIT_LEVEL(log)  do { \
(log)->level = (Log_level *)calloc(1, sizeof(Log_level)); \
if (NULL != (log)->level) L_INIT_HIGHLIGHT((log)->level->hl); \
}while(0)

/**
 * 初始化整个单条日志的结构
 */
#define L_INIT_LOG(log)   do { \
if (NULL == log) break; \
(log)->level = NULL; \
(log)->host = NULL; \
(log)->time = NULL; \
(log)->file = NULL; \
(log)->logid = 0; \
(log)->logidstr = NULL; \
(log)->extra = NULL; \
(log)->value = NULL; \
(log)->pos = 0; \
(log)->fhl = NULL; \
(log)->lhl = NULL; \
(log)->ehl = NULL; \
L_INIT_HIGHLIGHT((log)->fhl); \
L_INIT_HIGHLIGHT((log)->lhl); \
L_INIT_HIGHLIGHT((log)->ehl); \
} while(0)

/**
 * 判断高亮器是否已开启
 */
#define HAS_HIGHLIGHT(__hl)   ((NULL != __hl) && ((__hl)->pre != NULL || (__hl)->str != NULL || (__hl)->app != NULL))

/**
 * 字段赋值为整型
 */
#define LF_LONG(field, tmp) do { \
L_INIT_VALUE(field); \
L_SET_TYPE(field, TYPE_LONG); \
(field)->valstr->vallong = atol(tmp); \
(field)->valstr->valdbl = atof(tmp); \
(field)->valstr->valstring = (tmp); \
}while(0)

/**
 * 字段赋值为浮点型
 */
#define LF_DOUBLE(field, tmp) do { \
L_INIT_VALUE(field); \
L_SET_TYPE(field, TYPE_DOUBLE); \
(field)->valstr->valstring = (tmp); \
(field)->valstr->vallong = atol(tmp); \
(field)->valstr->valdbl = atof(tmp); \
}while(0)

/**
 * 字段赋值为时间型
 */
#define LF_TIME(field, tmp, ptm) do { \
L_INIT_VALUE(field); \
L_SET_TYPE(field, TYPE_TIME); \
(field)->valstr->valstring = (tmp); \
(field)->valstr->vallong = (ptm)->ts; \
(field)->valstr->valdbl = (double)(ptm)->ts; \
}while(0)

/**
 * 字段赋值为字符串
 */
#define LF_STRING(field, tmp) do { \
L_INIT_VALUE(field); \
(field)->valstr->valstring = (tmp); \
}while(0)


#define L_FIELD_SKEY(field, key)    do { \
field->key = key; \
}while(0)

/**
 * 向日志结构中增加一个字段结构
 */
#define L_ADD_FIELD(field)    do { \
Log_field *add; \
L_INIT_FIELD(add); \
(field)->next = add; \
add->prev = (field); \
(field) = (field)->next; \
}while(0)

int parse_field(Log_field *field, char *tmp);
void format_init(void);
void format_free(void);

void field_free(Log_field *f);
Log_field* field_duplicate(Log_field *f, const char *key);

void format(char *, unsigned long, bool failOutput);

/**
 * 取子字符串，并支持删除两边空格
 *
 * TODO 使用 lstring
 * @param str
 * @param len
 * @param trim 0保持原样；1删除空格
 * @return
 */
static char* sub_str_trim(const char *str, size_t len, unsigned char trim) {
    size_t reallen;
    char *copy;
    char *src = (char *)str;

    if (len <= 0) return 0;

    reallen = strlen(src);
    if (reallen < len) {
        len = reallen;
    }

    if (trim) {
        while (len >= 1 && is_spc(src)) {
            src++;
            len--;
        }
        while (len >= 1 && is_spcs(src + len - 1)) len--;
    }

    len++;

    if (!(copy = (char *) malloc(len))) return 0;

    if (*(src + len - 1) != '\0') len--;

    memcpy(copy, src, len);

    if (*(src + len) != '\0') *(copy + len) = '\0';

    return copy;
}

#define sub_str(str, len)  sub_str_trim(str, (size_t)(len), 0)
#define sub_trim(str, len)  sub_str_trim(str, (size_t)(len), 1)

#endif //LOGR_FORMAT_H
