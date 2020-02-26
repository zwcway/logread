//
// Created by Administrator on 2019/3/25.
//

#ifndef LOGR_FILTER_H
#define LOGR_FILTER_H


#include <regex.h>
#include "format.h"
#include "logr.h"
#include "lstring.h"

/** 正则匹配 */
#define F_OP_PG     1
/** 正则匹配 - 取反 */
#define F_OP_NPG    51
/** 模糊匹配 */
#define F_OP_FZ     2
/** 模糊匹配 - 取反 */
#define F_OP_NFZ    52

/** 小于 */
#define F_OP_LT     3
/** 大于 */
#define F_OP_GT     4
/** 大于等于 */
#define F_OP_GE     5
/** 小于等于 */
#define F_OP_LE     6
/** 等于 */
#define F_OP_EQ     7
/** 不等于 */
#define F_OP_NEQ    57
/** 是否存在KEY */
#define F_OP_KEY    97

#define F_OP_MASK   0xFFF

/** 标记是否是JSON的KEY */
#define F_OPT_JSONKEY   0x1000
/** 标记是否是数字型 */
#define F_OPT_NUMVAL    0x2000
#define F_OP_VAL  99

/** 完全匹配 */
#define FCT_NORMAL   0
/** 通配符匹配 */
#define FCT_WILDCARD 1
/** 标记为普通字段出匹配 */
#define FCF_TEXT     0x10
/** 标记为JSON路径匹配 */
#define FCF_JSON     0x20
/** 标记为JSON任意路径匹配 */
#define FCF_WILDJSON 0x40
/** 标记为JSON路径可唯一确定 */
#define FCF_DEFINITE 0x80

#define FCT_IS_TYPE(__ft, __t)        (((__ft)->type&0x0F) == __t)
#define FCT_HAS_FLAG(__ft, __t)        (((__ft)->type&__t) == __t)

#define FC_OR 1
#define FC_AND 2

/** 获取过滤器的操作符 */
#define F_GET_OP(__op)          ((__op)->op & F_OP_MASK)
/** 判断过滤器的操作符 */
#define F_IS_OP(__op, __t)      (F_GET_OP(__op) == __t)
/** 判断过滤器的类型 */
#define F_IS_TOPT(__op, __t)    ((__op)->type&__t)

/**
 * JSON输出标记 递归输出子JSON
 */
#define FC_OPT_RECURSE      0x2
/**
 * 通用标记 上一次列过滤匹配成功
 */
#define FC_OPT_LASTSUCC     0x1000

/**
 * 过滤失败，允许输出
 */
#define F_FAIL 0
/**
 * 过滤成功，禁止输出
 */
#define F_SUCC 1

/**
 * 过滤器结构体
 */
typedef struct Filter {
    char *key;
    unsigned int op;
    char *valstr;
    long long vallong;
    double valdbl;
    regex_t *reg;
    /** key的匹配方式 */
    unsigned int type;
} Filter;

/**
 * 过滤器列表 —— 链表
 */
typedef struct Filter_list {
    struct Filter_list *next;
    Filter *filter;
} Filter_list;

typedef struct Column_list {
    struct Column_list *next;
    const char *column;
    unsigned type;
    /** 与或 */
    unsigned char cond;
} Column_list;

/** 字符是否是操作符 */
#define IS_OP(chr)      ((chr)=='>'||(chr)=='<'||'='==(chr)||'!'==(chr)||'*'==(chr)||'~'==(chr))
/** 操作符是否是数字类 */
#define F_IS_NUMOP(ff)  (F_IS_OP(ff,F_OP_EQ)||F_IS_OP(ff,F_OP_LT)||F_IS_OP(ff,F_OP_LE)||F_IS_OP(ff,F_OP_GT)||F_IS_OP(ff,F_OP_GE)||F_IS_OP(ff,F_OP_NEQ))
/** 操作符是否是数字类 */
#define F_IS_NUM(ff)    (F_IS_TOPT(ff,F_OPT_NUMVAL)&&F_IS_NUMOP(ff))
/** 操作符是否是字符串类 */
#define F_IS_STROP(ff)  (F_IS_OP(ff,F_OP_FZ)||F_IS_OP(ff,F_OP_PG)||F_IS_OP(ff,F_OP_NFZ)||F_IS_OP(ff,F_OP_EQ)||F_IS_OP(ff,F_OP_NPG)||F_IS_OP(ff,F_OP_NEQ))
/** 操作符是否是字符串类 */
#define F_IS_STR(ff)    (!F_IS_TOPT(ff,F_OPT_NUMVAL)&&F_IS_STROP(ff))
/** 操作符是否是正则类 */
#define F_IS_PEGOP(ff)  (F_IS_OP(ff,F_OP_PG)||F_IS_OP(ff,F_OP_NPG))

#define FC_IS_SUCCESS(cur) (!(cur) || !(cur)->column || !(*(cur)->column))
#define FC_IS_END(cur)     ((!(cur) || !(cur)->next))

/**
 * 过滤器结构初始化 <br/>
 * filter : Filter *
 */
#define F_INIT(filter) do { \
filter = (Filter *)malloc(sizeof(Filter)); \
filter->key = 0; \
filter->op = 0; \
filter->valstr = 0; \
filter->valdbl = 0; \
filter->vallong = 0; \
filter->reg = 0; \
} while(0)

/**
 * 保存全局的过滤器列表
 */
extern Filter_list *fts;
/**
 * 保存全局的过滤列列表
 */
extern Column_list *cts;

typedef int (*print_colmn_func)(void *, const Log *, const Column_list *, const int);

/**
 * 仅用于正则
 */
regmatch_t pmatch[1];

extern void filter_free();
extern int collect_filter(const char*);
extern int collect_colmun(const char*, unsigned char);

extern Log_field*  filter_fieldcolumn(const Column_list *cur, Log_field *field);
extern int filter_column(const Column_list *, const char *, unsigned);
extern int filter_column_callback(void*, const Log *, int , print_colmn_func);

static void F_HL_P(Highlight *__hl, const char *__str, int __of, int __ln) {
    if (!__hl) return;

    if(__of) __hl->pre = sub_str(__str, __of);
    if(__ln) __hl->str = sub_str(__str + __of, __ln);
    __hl->app = sub_str(__str + __of + __ln, MAX_LINE);
}

/**
 * 过滤整型数据
 *
 * @param filter
 * @param lng
 * @return
 */
static int filter_long(const Filter *filter, const long long lng) {
    switch (F_GET_OP(filter)) {
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
        default:break;
    }
    return F_FAIL;
}
/**
 * 过滤浮点型数据
 *
 * @param filter
 * @param dbl
 * @return
 */
static int filter_double(const Filter *filter, const double dbl) {
    switch (F_GET_OP(filter)) {
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
        default:break;
    }
    return F_FAIL;
}
/**
 * 过滤字符串型数据
 * @param filter
 * @param str
 * @param hl 返回已匹配的字符串始末指针，可用于高亮
 * @return
 */
static int filter_string(const Filter *filter, const char *str, Highlight *hl) {
    if (str == 0) return F_FAIL;

    int ret = F_FAIL;
    const char *findstr = 0;

    switch (F_GET_OP(filter)) {
        // 正则
        case F_OP_PG:
            ret = regexec(filter->reg, str, 1, pmatch, 0) == REG_NOERROR;

            if (color_option && hl && ret)
                F_HL_P(hl, str, pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so);

            return ret;
        // 模糊匹配
        case F_OP_FZ:
            findstr = stristr(str, filter->valstr);
            ret = findstr != 0;

            if (color_option && hl && ret)
                F_HL_P(hl, str, (int) (findstr - str), (int) strlen(filter->valstr));

            return ret;
        // 正则取反
        case F_OP_NPG: return regexec(filter->reg, str, 1, pmatch, 0) == REG_NOMATCH;
        // 模糊取反
        case F_OP_NFZ: return stristr(str, filter->valstr) == 0;
        case F_OP_EQ:
            ret = strcasecmp(str, filter->valstr) == 0;

            if (color_option && hl && ret)
                F_HL_P(hl, str, 0, (int) strlen(str));

            return ret;
        case F_OP_NEQ:
            ret = strcasecmp(str, filter->valstr) != 0;

            if (color_option && hl && ret)
                F_HL_P(hl, str, 0, (int) strlen(str));

            return ret;
        // 仅判断键名是否存在
        case F_OP_KEY:
            return F_SUCC;
        default:break;
    }

    return F_FAIL;
}
/**
 * 过滤json值
 *
 * @param filter
 * @param json
 * @return
 */
static int filter_json(const Filter *filter, const cJSON *json) {
    if (json == 0) return F_FAIL;
    if (!filter->key) return F_FAIL;
    if (!filter->valstr) return F_FAIL;
    cJSON *item = (cJSON*)json;

    // TODO 使用循环代替递归
    for(; item; item = item->next) {
        switch (item->type) {
            case cJSON_Object:
            case cJSON_Array:
                if (item->child) {
                    if (F_SUCC == filter_json(filter, item->child)) return F_SUCC;
                }
                break;
            case cJSON_Number:
                if (F_SUCC == filter_double(filter, item->valuedouble)) return F_SUCC;
                break;
            case cJSON_String:
                if (F_SUCC == filter_string(filter, item->valuestring, 0)) return F_SUCC;
                break;
            case cJSON_False:
            case cJSON_True:
                if (F_SUCC == filter_long(filter, item->valueint)) return F_SUCC;
                break;
            case cJSON_NULL:
                if (F_SUCC == filter_long(filter, 0)) return F_SUCC;
                break;
            default:
                break;
        }
    }
    return F_FAIL;
}
/**
 * 判断json的路径是否存在
 *
 * @param filter
 * @param json
 * @return
 */
static int filter_jsonkey(const Filter *filter, const cJSON *json) {
    if (json == 0) return F_FAIL;
    if (!filter->key) return F_FAIL;
    cJSON *item = (cJSON*)json;

    // TODO 使用循环代替递归
    for (; item; item = item->next) {
        switch (item->type) {
            case cJSON_Object:
            case cJSON_Array:
                if (item->path) {
                    if (0 == strcasecmp(filter->key, item->path)) return F_SUCC;
                }
                if (item->child) {
                    if (F_SUCC == filter_jsonkey(filter, item->child)) return F_SUCC;
                }
                break;
            default:
                if (item->path) {
                    if (0 == strcasecmp(filter->key, item->path)) return F_SUCC;
                }
                break;
        }
    }
    return F_FAIL;
}

static int filter_time(const Filter *filter, const Log_value *time) {
    if (!time) return F_FAIL;

    if (F_IS_NUM(filter)) return filter_long(filter, (long long)time->vallong);
    return filter_string(filter, time->valstring, 0);
}
static int filter_host(const Filter *filter, Log_host *host) {
    if (!host) return F_FAIL;

    if (F_IS_NUM(filter)) return filter_long(filter, host->lip);
    return filter_string(filter, host->ip, host->hl);
}
static int filter_level(const Filter *filter, Log_level *level) {
    if (!level) return F_FAIL;
    if (F_IS_NUM(filter)) return filter_long(filter, level->lint);
    return filter_string(filter, level->lstr, level->hl);
}

static int filter_field(const Filter *filter, Log_field *field) {
    if (field == NULL || field->valstr == NULL) return F_FAIL;

    if (field->type == TYPE_JSON) {
        if (filter->key && F_IS_TOPT(filter, F_OPT_JSONKEY)) return filter_json(filter, field->valjson);
        return filter_string(filter, field->valstr->valstring, field->hl);
    }

    if (field->type == TYPE_NULL) {
        if (F_IS_OP(filter, F_OP_KEY)) return F_SUCC;
        return F_FAIL;
    }

    if (F_IS_NUM(filter)) {
        if (field->type == TYPE_DOUBLE) return filter_double(filter, field->valstr->valdbl);
        if (field->type == TYPE_LONG) return filter_long(filter, field->valstr->vallong);
    }
    if (F_IS_STR(filter)) {
        return filter_string(filter, field->valstr->valstring, field->hl);
    }

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
        if (field->key) {
            // 只有过滤器中包含jsonkey分隔符的时候，在判断。减少不必要的循环
            if (field->type == TYPE_JSON && F_IS_TOPT(filter, F_OPT_JSONKEY)) {
                // 匹配json key
                if (filter_jsonkey(filter, field->valjson)) return F_SUCC;
            } else {
                if (0 == strcasecmp(filter->key, field->key)) return F_SUCC;
            }
        }

    return F_FAIL;
}

/**
 * 任意键名，是否存在某个值，存在即成功
 * @param filter
 * @param log
 * @return
 */
static int filter_value(const Filter *filter, const Log *log) {
    Log_field *field = log->value;

    if (log->host && filter_host(filter, log->host)) return F_SUCC;
    if (log->level && filter_level(filter, log->level)) return F_SUCC;
    if (filter_long(filter, log->logid)) return F_SUCC;
    if (log->file && filter_string(filter, log->file, log->fhl)) return F_SUCC;
    if (log->time && filter_time(filter, log->time)) return F_SUCC;

    for(;field;field=field->next)
        if (filter_field(filter, field)) return F_SUCC;

    if (log->extra && filter_string(filter, log->extra, log->ehl)) return F_SUCC;

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
            if (F_IS_OP(ff, F_OP_KEY))
                continue;

            if (0 == strcasecmp(ff->key, COL_HOST) && !filter_host(ff, log->host))
                return F_FAIL;
            if (0 == strcasecmp(ff->key, COL_LEVEL) && !filter_level(ff, log->level))
                return F_FAIL;
            if (0 == strcasecmp(ff->key, COL_LOGID) && !filter_long(ff, log->logid))
                return F_FAIL;
            if (0 == strcasecmp(ff->key, COL_FILE) && !filter_string(ff, log->file, log->fhl))
                return F_FAIL;
            if (0 == strcasecmp(ff->key, COL_TIME) && !filter_time(ff, log->time))
                return F_FAIL;

            for (;field;field=field->next)
                if (field->key && 0 == strcasecmp(ff->key, field->key) && !filter_field(ff, field))
                    return F_FAIL;

            if (0 == strcasecmp(ff->key, COL_EXTRA) && !filter_string(ff, log->extra, log->ehl))
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
