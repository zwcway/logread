//
// Created by Administrator on 2019/3/25.
//

#include <stdlib.h>
#include "filter.h"
#include "cJSON.h"
#include "format.h"
#include "utils.h"

Filter_list *fts;
Filter_list *fts_cur;


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

void filter_free(Filter_list *filter) {
    Filter_list *next;
    Filter_list *cur = filter;

    while (cur) {
        next = cur->next;

        f_free(cur->filter);
        free(cur);

        cur = next;
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
        fts_cur = fts = malloc(sizeof(Filter_list));
    } else {
        fts_cur->next = malloc(sizeof(Filter_list));
        fts_cur = fts_cur->next;
    }
    fts_cur->next = 0;
    fts_cur->filter = filter;

    return 1;
}