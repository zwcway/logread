//
// Created by Administrator on 2019/3/25.
//

#include <stdlib.h>
#include "filter.h"
#include "cJSON.h"
#include "format.h"
#include "utils.h"

Filter_list *fts = 0;
Filter_list *fts_cur = 0;
Column_list *cts = 0;
Column_list *cts_cur = 0;


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
    Column_list *ccur;

    while (fts) {
        fcur = fts->next;

        if (fts->filter) f_free(fts->filter);
        free(fts);

        fts = fcur;
    }

    while (cts) {
        ccur = cts->next;

//        if (cts->column) free(cts->column);
        free(cts);

        cts = ccur;
    }
}

/**
 * 支持json
 * @param key
 */
void parse_key(const char *key) {

}

unsigned char parse_filter(Filter *filter, const char *str) {
    char *start = (char *)str;
    char nextchr;

    if (is_end(str)) return 0;

    while (!IS_OP(*str) && !is_end(str)) str++;

    filter->key = sub_trim(start, str - start);

    nextchr = *(str + 1);

    if (*str == '>') {
        if (*(str + 1) == '=') {
            filter->op = F_OP_GE;
            str++;
        } else filter->op = F_OP_GT;
    } else if (*str == '<') {
        if (nextchr == '=') {
            filter->op = F_OP_LE;
            str++;
        } else if (nextchr == '>') {
            filter->op = F_OP_NEQ;
            str++;
        } else filter->op = F_OP_LT;
    } else if (*str == '=') filter->op = F_OP_EQ;
    else if (*str == '~') filter->op = F_OP_PG;
    else if (*str == '*') filter->op = F_OP_FZ;
    else if (*str == '!') {
        if (nextchr == '>') filter->op = F_OP_LE;
        else if (nextchr == '<') filter->op = F_OP_GE;
        else if (nextchr == '=') filter->op = F_OP_NEQ;
        else if (nextchr == '~') filter->op = F_OP_NPG;
        else if (nextchr == '*') filter->op = F_OP_NFZ;
        str ++;
    }

    start = (char *)++str;
    while (!is_end(str)) str++;

    if (IS_NUMOP(filter->op)) {
        start = sub_trim(start, str - start);
        if (start) {
            filter->vallong = atoll(start);
            filter->valdbl = atof(start);
            free(start);
        }
    } else if(IS_STROP(filter->op)) {
        filter->valstr = sub_trim(start, str - start);
        if (filter->valstr && IS_PEGOP(filter->op)) {
            filter->reg = (regex_t *)malloc(sizeof(regex_t));
            if (regcomp(filter->reg, filter->valstr, REG_EXTENDED)) {
                regfree(filter->reg);
                free(filter->reg);
                filter->reg = 0;
            }
        }
    } else {
        if (filter->key) filter->op = F_OP_KEY;
        if (strstr(filter->key, CJSON_PATH_DELIMITER)) filter->op = F_OP_JSONKEY;
        else return 0;
    }

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

void add_column(char *c, unsigned char type, unsigned char cond) {
    if (!cts_cur)
        cts_cur = cts = (Column_list*)calloc(1, sizeof(Column_list));
    else {
        cts_cur->next = (Column_list*)calloc(1, sizeof(Column_list));
        cts_cur = cts_cur->next;
    }
    cts_cur->next = 0;
    cts_cur->column = skip(c);
    cts_cur->type = type;
    cts_cur->cond = cond;
}

int collect_colmun(const char *c, unsigned char cond) {
    char *str = (char*)c;
    char *col = strtok(str, ",");
    size_t len = 0;
    unsigned char type = FC_NORMAL;
    while(col) {
        if (col[0] == '*') {
            type |= FC_RIGHT;
            col++;
        }
        len = strlen(col) - 1;
        if (col[len] == '*') {
            type |= FC_LEFT;
            col[len] = '\0';
        }
        if (strchr(col, '.')) {
            type |= FC_JSON;
        }
        add_column(col, type, cond);
        col = strtok(NULL, ",");
    }
    return 1;
}

const cJSON* filter_jsoncolumn(const Column_list *cur, const cJSON *json) {
    if (!json) return NULL;
    if (!cur) return NULL;

    cJSON *item = (cJSON*)json;
    const cJSON *child;

    // TODO 使用循环代替递归
    for (; item; item = item->next) {
        switch (item->type) {
            case cJSON_Object:
            case cJSON_Array:
                if (item->path) {
                    if (F_SUCC == filter_column(cur, item->path)) return item;
                }
                if (item->child) {
                    if ((child = filter_jsoncolumn(cur, item->child))) return child;
                }
                break;
            default:
                if (item->path) {
                    if (F_SUCC == filter_column(cur, item->path)) return item;
                }
                break;
        }
    }

    return NULL;
}

cJSON* filter_json_duplicate(const cJSON *json) {
    cJSON *copy = cJSON_Duplicate((cJSON*)json, true);

    free(copy->string);
    copy->string = NULL;

    return copy;
}

/**
 *
 * @param cur
 * @param field
 * @return
 */
Log_field* filter_fieldcolumn(const Column_list *cur, Log_field *field) {
    if (!cur || !field) return NULL;

    if ((cur->type&FC_JSON) && field->type == TYPE_JSON) {
        const cJSON *json;
        if((json = filter_jsoncolumn(cur, field->valjson))) {
            Log_field *newfield = field_duplicate(field);
            newfield->valjson = filter_json_duplicate((cJSON*)json);
            return newfield;
        }
    }
    if (F_SUCC == filter_column(cur, field->key)) {
        return field;
    }

    return NULL;
}
/**
 * 过滤列名
 *
 * @param cur
 * @param key
 * @return F_SUCC：允许输出列
 */
int filter_column(const Column_list *cur, const char *key) {
    if (!cur || !key) return F_FAIL;

    if((cur->type & FC_RIGHT) && (cur->type & FC_LEFT)) {
        if (!stristr(key, cur->column)) return F_FAIL;
    } else if (cur->type & FC_RIGHT) {
        if (!striright(key, cur->column)) return F_FAIL;
    } else if (cur->type & FC_LEFT) {
        if (!strileft(key, cur->column)) return F_FAIL;
    } else {
        if (0 != strcasecmp(key, cur->column)) return F_FAIL;
    }

    return F_SUCC;
}

int  filter_column_callback(void* arg, const Log *log, const int opt, print_colmn_func func) {
    int count = 0, lastcount = -1;
    if (cts) {
        for (Column_list *cur = cts; cur; cur = cur->next) {
            if (cur->cond == FC_AND && lastcount == 0) return 0;
            lastcount = func(arg, log, cur, lastcount > 0 ? opt | FC_OPT_LASTSUCC : opt);
            count += lastcount;
        }
    } else {
        count += func(arg, log, 0, opt);
    }

    return count;
}