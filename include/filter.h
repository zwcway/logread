//
// Created by Administrator on 2019/3/25.
//

#ifndef LOGR_FILTER_H
#define LOGR_FILTER_H


#include <regex.h>
#include "format.h"
#include "logr.h"

/** 正则匹配 */
#define F_OP_PG  1
/** 正则匹配 - 取反 */
#define F_OP_NPG  51
/** 模糊匹配 */
#define F_OP_FZ  2
/** 模糊匹配 - 取反 */
#define F_OP_NFZ  52

/** 小于 */
#define F_OP_LT  3
/** 大于 */
#define F_OP_GT  4
/** 大于等于 */
#define F_OP_GE  5
/** 小于等于 */
#define F_OP_LE  6
/** 等于 */
#define F_OP_EQ  7
/** 不等于 */
#define F_OP_NEQ  57
/** 是否存在KEY */
#define F_OP_KEY  98
#define F_OP_VAL  99


#define F_FAIL 0
#define F_SUCC 1

typedef struct Filter {
    char *key;
    unsigned char op;
    char *valstr;
    long long vallong;
    double valdbl;
    regex_t *reg;
} Filter;

typedef struct Filter_list {
    struct Filter_list *next;
    Filter *filter;
} Filter_list;

#define IS_OP(chr)  (chr=='>'||chr=='<'||'='==chr||'!'==chr||'*'==chr||'~'==chr)
#define IS_NUMOP(op)  (F_OP_EQ==op||F_OP_LT==op||F_OP_LE==op|| F_OP_GT==op||F_OP_GE==op||F_OP_NEQ==op)
#define IS_STROP(op)  (F_OP_FZ==op||F_OP_PG==op||F_OP_NFZ==op||F_OP_NPG==op)
#define IS_PEGOP(op)  (F_OP_PG==op||F_OP_NPG==op)


#define F_INIT(filter) do { \
filter = (Filter *)malloc(sizeof(Filter)); \
filter->key = 0; \
filter->op = 0; \
filter->valstr = 0; \
filter->valdbl = 0; \
filter->vallong = 0; \
filter->reg = 0; \
} while(0)

extern Filter_list *fts;
regmatch_t pmatch[1];

extern void filter_free(Filter_list *filter);
extern int collect_filter(const char* f);

static int filter_long(const Filter *filter, const long long lng) {
    switch (filter->op) {
        case F_OP_LT:
            return lng < filter->vallong;
        case F_OP_LE:
            return lng <= filter->vallong;
        case F_OP_GT:
            return lng > filter->vallong;
        case F_OP_GE:
            return lng >= filter->vallong;
        case F_OP_NEQ:
            return lng != filter->vallong;
        case F_OP_EQ:
            return lng == filter->vallong;
        case F_OP_KEY:
            return F_SUCC;
    }
    return F_FAIL;
}

static int filter_double(const Filter *filter, const double dbl) {
    switch (filter->op) {
        case F_OP_LT:
            return dbl < filter->valdbl;
        case F_OP_LE:
            return dbl <= filter->valdbl;
        case F_OP_GT:
            return dbl > filter->valdbl;
        case F_OP_GE:
            return dbl >= filter->valdbl;
        case F_OP_NEQ:
            return dbl != filter->valdbl;
        case F_OP_EQ:
            return dbl == filter->valdbl;
        case F_OP_KEY:
            return F_SUCC;
    }
    return F_FAIL;
}

static int filter_string(const Filter *filter, const char *str) {
    if (str == 0) return F_FAIL;

    switch (filter->op) {
        case F_OP_PG: return regexec(filter->reg, str, 1, pmatch, 0) == REG_NOERROR;
        case F_OP_FZ: return strstr(str, filter->valstr) != 0;
        case F_OP_NPG: return regexec(filter->reg, str, 1, pmatch, 0) == REG_NOMATCH;
        case F_OP_NFZ: return strstr(str, filter->valstr) == 0;
        case F_OP_KEY: return F_SUCC;
    }
    return F_FAIL;
}

static int filter_json(const Filter *filter, const cJSON *json) {
    if (json == 0) return F_FAIL;

    return F_FAIL;
}

static int filter_time(const Filter *filter, const Log_time *time) {
    if (!time) return F_FAIL;

    if (IS_NUMOP(filter->op)) return filter_long(filter, (long long)time->ts);

    return filter_string(filter, time->str);
}
static int filter_host(const Filter *filter, const Log_host *host) {
    if (!host) return F_FAIL;

    return F_FAIL;
}
static int filter_level(const Filter *filter, const Log_level *level) {
    if (!level) return F_FAIL;
    return F_FAIL;
}

static int filter_field(const Filter *filter, const Log_field *field) {
    if (field == 0) return F_FAIL;

    if (field->type == TYPE_JSON)
        return filter_json(filter, field->val.valjson);

    if (field->type == TYPE_NULL)
        if (filter->op == F_OP_KEY) return F_SUCC;
        else return F_FAIL;

    if (IS_NUMOP(filter->op))
        if (field->type == TYPE_DOUBLE) return filter_double(filter, field->val.valstr->valdbl);
        else if(field->type == TYPE_LONG) return filter_long(filter, field->val.valstr->vallong);
    if (IS_STROP(filter->op))
        return filter_string(filter, field->val.valstr->valstring);

    return F_FAIL;
}

/**
 * 是否存在键名
 * @param filter
 * @param log
 * @return 1表示存在；0表示不存在
 */
static unsigned filter_key(const Filter *filter, const Log *log) {
    Log_field *field = log->value;

    if ((log->host && 0 == strcasecmp(filter->key, COL_HOST)) ||
        (log->level && 0 == strcasecmp(filter->key, COL_LEVEL)) ||
        (0 == strcasecmp(filter->key, COL_LOGID)) ||
        (log->time && 0 == strcasecmp(filter->key, COL_TIME)) ||
        (0 == strcasecmp(filter->key, COL_EXTRA)))
        return F_SUCC;

    for (;field;field=field->next)
        if (field->key)
            if (0 == strcasecmp(filter->key, field->key))
                return F_SUCC;

    return F_FAIL;
}

/**
 * 是否存在某个值，存在即成功
 * @param filter
 * @param log
 * @return
 */
static int filter_value(const Filter *filter, const Log *log) {
    Log_field *field = log->value;

    if (log->host && filter_host(filter, log->host)) return F_SUCC;
    if (log->level && filter_level(filter, log->level)) return F_SUCC;
    if (filter_long(filter, log->logid)) return F_SUCC;
    if (log->time && filter_time(filter, log->time)) return F_SUCC;
    if (log->extra && filter_string(filter, log->extra)) return F_SUCC;

    for(;field;field=field->next)
        if (filter_field(filter, field)) return F_SUCC;

    return F_FAIL;
}

/**
 *
 * @param log
 * @return 0 表示过滤成功；1 表示无需过滤
 */
static int filter_log(const Log *log) {
    Filter_list *filter = fts;
    Log_field *field = log->value;
    Filter *ff;

    for(;filter; filter=filter->next) {
        ff = filter->filter;
        // 使用了键名过滤
        if (ff->key) {
            // 仅仅校验键名是否存在，如果有一个条件不存在，就表示过滤成功
            if (!filter_key(ff, log)) return F_FAIL;
            if (ff->op == F_OP_KEY)
                continue;

            if (0 == strcasecmp(ff->key, COL_HOST) && !filter_host(ff, log->host))
                return F_FAIL;
            if (0 == strcasecmp(ff->key, COL_LEVEL) && !filter_level(ff, log->level))
                return F_FAIL;
            if (0 == strcasecmp(ff->key, COL_LOGID) && !filter_long(ff, log->logid))
                return F_FAIL;
            if (0 == strcasecmp(ff->key, COL_TIME) && !filter_time(ff, log->time))
                return F_FAIL;
            if (0 == strcasecmp(ff->key, COL_EXTRA) && !filter_string(ff, log->extra))
                return F_FAIL;

            for (;field;field=field->next)
                if (field->key && 0 == strcasecmp(ff->key, field->key) && !filter_field(ff, field))
                    return F_FAIL;

        } else {
            // 仅仅校验值是否存在，如果有一个条件不存在，就表示匹配失败（过滤成功）
            if (!filter_value(ff, log))
                return F_FAIL;
            continue;
        }
    }

    return F_SUCC;
}

#endif //LOGR_FILTER_H
