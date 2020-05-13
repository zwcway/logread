//
// Created by Administrator on 2019/3/25.
//

#include <stdlib.h>
#include "type/time.h"
#include "filter.h"
#include "cJSON.h"
#include "format.h"
#include "utils.h"

Filter_list *fts = 0;
Filter_list *fts_cur = 0;
/**
 * 列过滤列表
 */
Column_list *cts = 0;
/**
 * 列过滤长度
 */
static int cts_len = 0;
static int cts_size = 0;

#define CTS_MAX_LEN 30
#define CTS_JSON_MAX_LEN 10

void f_free(Filter *f) {
    if (f) {
        if (f->reg) {
            regfree(f->reg);
            free(f->reg);
        }
        if (f->valstr) free(f->valstr);
        if (f->key) free(f->key);
        free(f);
    }
}

void filter_free() {
    Filter_list *fcur;
    Column_list *ccur, *ncur;

    while (fts) {
        fcur = fts->next;

        if (fts->filter) f_free(fts->filter);
        free(fts);

        fts = fcur;
    }

    while (cts_len--) {
        ccur = (cts + cts_len)->next;
        while (ccur) {
            ncur = ccur->next;
            free(ccur);
            ccur = ncur;
        }
    }
    free(cts);
}
/**
 * 预处理过滤器，解析成方便计算的格式
 *
 * @param filter
 * @param str
 * @return
 */
unsigned int parse_filter(Filter *filter, const char *str) {
    char *start = (char *)str;
    char nextchr;
    int guess = 0;
    Time time;

    if (is_end(str)) return 0;

    while (!IS_OP(*str) && !is_end(str)) str++;

    filter->key = sub_trim(start, str - start);

    nextchr = *(str + 1);

    if (*str == '>') {
        if (nextchr == '=') {           // >=
            filter->op = F_OP_GE;
            str++;
        } else                          // >
            filter->op = F_OP_GT;
    } else if (*str == '<') {
        if (nextchr == '=') {           // <=
            filter->op = F_OP_LE;
            str++;
        } else if (nextchr == '>') {    // <>
            filter->op = F_OP_NEQ;
            str++;
        } else                          // <
            filter->op = F_OP_LT;
    } else if (*str == '=')
        if (nextchr == '=') {           // ==
            filter->op = F_OP_EQ;
            str++;
        } else                          // =
            filter->op = F_OP_EQ;
    else if (*str == '~')               // ~
        filter->op = F_OP_PG;
    else if (*str == '*')               // *
        filter->op = F_OP_FZ;
    else if (*str == '!') {
        str++;
        if (nextchr == '>')             // !>
            filter->op = F_OP_LE;
        else if (nextchr == '<')        // !<
            filter->op = F_OP_GE;
        else if (nextchr == '=')        // !=
            filter->op = F_OP_NEQ;
        else if (nextchr == '~')        // !~
            filter->op = F_OP_NPG;
        else if (nextchr == '*')        // !*
            filter->op = F_OP_NFZ;
        else {                          // !
            filter->op = F_OP_NEQ;
            str --;
        }
    }

    start = (char *)++str;
    while (!is_end(str)) str++;

    filter->valstr = sub_trim(start, str - start);

    guess = guessType(filter->valstr, &time);

    // 根据入参判断过滤器值的类型
    switch (guess)  {
        case TYPE_LONG:
        case TYPE_DOUBLE:
        case TYPE_TIME:
            filter->type |= F_OPT_NUMVAL;
            break;
        default:break;
    }

    if(F_IS_STROP(filter)) {
        if (filter->valstr && F_IS_PEGOP(filter)) {
            filter->reg = (regex_t *)malloc(sizeof(regex_t));
            if (regcomp(filter->reg, filter->valstr, REG_EXTENDED)) {
                regfree(filter->reg);
                free(filter->reg);
                filter->reg = 0;
            }
        }
    }
    if (F_IS_NUMOP(filter)) {
        if (guess == TYPE_TIME) {
            filter->vallong = time.ts;
        }
        else if (filter->valstr) {
            filter->vallong = atoll(filter->valstr);
            filter->valdbl = atof(filter->valstr);
        }
    }
    if (filter->key && NULL == filter->valstr)
        filter->op = F_OP_KEY;

    if (filter->key && strstr(filter->key, CJSON_PATH_DELIMITER))
        filter->type |= F_OPT_JSONKEY;

    return filter->op;
}

int collect_filter(const char *f) {
    Filter *filter;

    F_INIT(filter);

    if (!parse_filter(filter, skip(f))) {
        f_free(filter);
        return 0;
    }

    if (!fts_cur) {
        // 首次初始化
        fts_cur = fts = (Filter_list*)calloc(1, sizeof(Filter_list));
    } else {
        fts_cur->next = (Filter_list*)calloc(1, sizeof(Filter_list));
        fts_cur = fts_cur->next;
    }
    fts_cur->next = 0;
    fts_cur->filter = filter;

    return 1;
}
/**
 * 添加一个列过滤选项
 * @param c
 * @param type
 * @param cond
 */
void add_column(char *c, unsigned type, unsigned char cond) {
    if (cts_len++ >= CTS_MAX_LEN) {
        return;
    }
    cts_size = 10 * ((cts_len / 10) + 1);
    if (!cts || cts_len > cts_size) {
        cts = (Column_list *) realloc(cts, cts_size * sizeof(Column_list));
    }

    Column_list *cts_cur = (cts + (cts_len - 1));
    unsigned char depth = 0;
    unsigned j_type = 0;
    bool definite = true;

    // json格式
    if (strchr(c, '.'))
        type |= FCF_JSON;
    else
        type |= FCF_TEXT;

    char *jc = c;
    do {
        c = strchr(jc, '.');
        if (c) {
            *c = '\0';
            c++;
        }
        if (depth > CTS_JSON_MAX_LEN) return;

        j_type = type;
        // 仅支持这几种通配符
        if (strchr(jc, '*') || strchr(jc, '?')) {
            j_type |= FCT_WILDCARD;

            if (depth) {
                definite = false;
            }
            if (j_type & FCF_JSON && strstr(jc, "**"))
                j_type |= FCF_WILDJSON;
        }

        if (depth ++ > 0) {
            cts_cur->next = (Column_list *) calloc(1, sizeof(Column_list));
            cts_cur = cts_cur->next;
        }

        cts_cur->next = 0;
        cts_cur->type = j_type;
        cts_cur->cond = FC_AND;
        cts_cur->column = skip(jc);

        jc = c;
    } while(jc);

    if (!definite)
        return;

    // 设置确定性标记
    cts_cur = (cts + (cts_len - 1));
    while (cts_cur) {
        cts_cur->type |= FCF_DEFINITE;
        cts_cur = cts_cur->next;
    }

}
/**
 * 将运行参数中的列过滤选项添加至全局列过滤列表中
 * @param c
 * @param cond or and
 * @return
 */
int collect_colmun(const char *c, unsigned char cond) {
    char *str = (char*)c;
    unsigned type;
    unsigned count = (unsigned) (char_count(c, ',') + 1);

    char *col = strtok(str, ",");
    while(col) {
        type = FCT_NORMAL;
        add_column(col, type, cond);
        col = strtok(NULL, ",");
    }
    return count;
}

/**
 * 复制出一个新的json节点
 * @param json
 * @return
 */
cJSON* filter_json_duplicate(const cJSON *json) {
    cJSON *copy = cJSON_Duplicate((cJSON*)json, true);

    return copy;
}

/**
 * 过滤json 取完整Key值
 *
 * TODO 支持任意路径匹配 a.**.c
 * TODO 最小化展示json  isdefinite
 * @param cur
 * @param json
 * @return 返回找到的匹配的节点
 */
cJSON* filter_jsoncolumn(const Column_list *cur, const cJSON *json) {
    if (!json) return NULL;

    const cJSON *item = json;
    /* 保存最终结果的 json 对象 */
    cJSON *finded_json = NULL, *finded_item = NULL, *finded_child = NULL;
    bool finded = false;

    if (FC_IS_SUCCESS(cur) && FC_IS_END(cur)) {
        // 过滤列表已经到达末尾，返回所有子节点
        finded_json = finded_item = cJSON_Duplicate(item, true);
        return finded_json;
    }

    bool isWild = FCT_IS_TYPE(cur, FCT_WILDCARD);
    bool isdefinite = !logr_fullcol && FCT_HAS_FLAG(cur, FCF_DEFINITE);

    do {
        // 必须存在json路径属性
        if (!item->path) continue;

        if (item->string) {
            if (F_FAIL == filter_column(cur, item->string, FCF_JSON)) continue;
        } else {
            // 过滤第一个节点
            if (F_FAIL == filter_column(cur, item->path, FCF_JSON)) break;
        }

        finded_child = cJSON_Duplicate(item, false);
        if (!finded_json) {
            // 准备好新json对象，复制第一个节点
            finded_json = finded_item = finded_child;
        } else {
            // 准备好下一个节点
            finded_item->next = finded_child;
            finded_item = finded_item->next;
        }

        switch (item->type) {
            case cJSON_Object:
            case cJSON_Array:
                // 递归过滤所有子节点
                finded_child = filter_jsoncolumn(cur->next, item->child);
                if (finded_child) {
                    if (logr_fullcol) {
                        // 插入到 准备好的节点中
                        cJSON_AddItemToArray(finded_item, finded_child);
                    } else {
                        cJSON_Delete(finded_json);
                        finded_json = finded_child;
                    }
                    finded = true;
                }
                continue;
            default:
                break;
        }
        if (cur->next) {
            // 过滤条件中包含子层级，但是原json中已经没有子节点了
            finded = false;
        } else {
            finded = true;
        }
    } while (item = item->next);

    // 没有找到相匹配的item，释放内存
    if (!finded && finded_json) {
        cJSON_Delete(finded_json);
        finded_json = NULL;
    }

    return finded_json;
}

/**
 * 根据字段过滤
 * @param cur
 * @param field
 * @return 使用完一定要记得field_free
 */
Log_field* filter_fieldcolumn(const Column_list *cur, Log_field *field) {
    if (!field) return NULL;
    if (!field->key) return NULL;
    if (FC_IS_SUCCESS(cur)) return field;

    if ((cur->type&FCF_JSON)) {
        if (field->type != TYPE_JSON) return NULL;
        cJSON *json;
        if((json = filter_jsoncolumn(cur, field->valjson))) {
            Log_field *newfield = field_duplicate(field, field->key);
            newfield->valjson = json;
            return newfield;
        }
    } else {
        if (F_SUCC == filter_column(cur, field->key, FCF_TEXT)) return field;
    }

    if (field->key) free(field->key);
    field->key = NULL;

    return NULL;
}
/**
 * 过滤字段名称
 *
 * @param cur
 * @param key
 * @return F_SUCC：允许输出列
 */
int filter_column(const Column_list *cur, const char *key, unsigned type) {
    if (!key) return F_FAIL;
    if (FC_IS_SUCCESS(cur)) return F_SUCC;
    if (!(cur->type & type)) return F_FAIL;

    if(FCT_IS_TYPE(cur, FCT_WILDCARD)) {
        if (is_match(key, cur->column)) return F_SUCC;
    } else if(FCT_IS_TYPE(cur, FCT_NORMAL)) {
        if (0 == strcasecmp(key, cur->column)) return F_SUCC;
    }

    return F_FAIL;
}

int  filter_column_callback(void* arg, const Log *log, const int opt, print_colmn_func func) {
    int count = 0, lastcount = -1, len;
    if (cts) {
        Column_list * cur;
        for (len = 0; len < cts_len; len ++) {
            cur = cts + len;
            if (cur->cond == FC_AND && lastcount == 0) return 0;
            lastcount = func(arg, log, cur, lastcount > 0 ? opt | FC_OPT_LASTSUCC : opt);
            count += lastcount;
        }
    } else {
        count += func(arg, log, 0, opt);
    }

    return count;
}