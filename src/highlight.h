//
// Created by Administrator on 2019/3/13.
//

#ifndef LOGR_HIGHLIGHT_H
#define LOGR_HIGHLIGHT_H

#include "color.h"
#include "format.h"
#include "lstring.h"
#include "logr.h"
#include "output.h"

#define MAX_COLORS 128
#define MAX_BUFFER_SIZE 64

#define HL_START    "\33[%sm\33[K"
#define HL_END      "\33[m\33[K"

#define HL_INTERNAL_KEY         "ky"
#define HL_INTERNAL_VAL         "vl"
#define HL_INTERNAL_BACKGROUND  "bg"
#define HL_INTERNAL_OPERATOR    "eq"

static const char *logr_op = "=";
static const char *logr_spc = " ";

/* 内置的基础颜色值变量 */
static char *field_key_color = C_CYAN;
static char *field_val_color = C_L_GRAY;
static char *background_color = C_NONE;
static char *operator_color  = C_BROWN;

typedef struct color_cap {
    char *name;
    char **var;

    void (*fct)(void);
} color_cap;

typedef struct colors_map {
    char *name;
    char *var;
} colors_map;


static colors_map colors_dict[MAX_COLORS];
static int colors_dict_len;

static char _strbuffer[MAX_BUFFER_SIZE];

inline static const char* fetch_color(const char *__name, const char *__default) {
    if (!color_option || NULL == __name) return 0;

    // 优先使用配置颜色值
    colors_map const *cllet;
    for (cllet = colors_dict; cllet->name; cllet++) {
        if (STREQ (cllet->name, __name))
            return cllet->var;
    }
    return __default;
}

#define PRINTF_LENGTH       MAX_LINE - (outputstr - *(__str))

#define SPRTF_STR_COLOR(__str, color, __val)        (*(__str)) += snprintf(*(__str), PRINTF_LENGTH, HL_START "%s" HL_END, color, __val)
#define SPRTF_STR(__str, __val)                     (*(__str)) += snprintf(*(__str), PRINTF_LENGTH, "%s", __val)

#define SPRTF_STR_BUF(__str, __len, __fmt, __val)   if (__val) (*(__str)) += snprintf(*(__str), __len, __fmt, __val)

/**
 * 支持自定义颜色输出日志
 *
 * @param __str
 * @param __key
 * @param __val
 * @param print_space
 */
static void sprtf_key_val (char **__str, const char *__key, const char *__val, const bool print_space) {
    if (color_option) {
        const char *color;

        // 拼接全局的颜色处理
        if (__val) {
            snprintf(_strbuffer, MAX_BUFFER_SIZE, "%s%s%s", __key, ",", __val);
        } else {
            snprintf(_strbuffer, MAX_BUFFER_SIZE, "%s%s", __key, ",");
        }

        if ( (color = fetch_color(_strbuffer, 0)) )
            SPRTF_STR_COLOR(__str, color, _strbuffer);
        else {
            color = fetch_color(HL_INTERNAL_KEY, field_key_color);
            if (color) SPRTF_STR_COLOR(__str, color, __key);
            else SPRTF_STR(__str, __val);

            color = fetch_color(HL_INTERNAL_OPERATOR, operator_color);
            if (color) SPRTF_STR_COLOR(__str, color, logr_op);
            else SPRTF_STR(__str, logr_op);

            if (__val) {
                color = fetch_color(HL_INTERNAL_VAL, field_val_color);
                if (color) SPRTF_STR_COLOR(__str, color, __val);
                else SPRTF_STR(__str, __val);
            }
        }
    } else {
        SPRTF_STR(__str, __key);
        SPRTF_STR(__str, logr_op);
        if (__val) SPRTF_STR(__str, __val);
    }

    if (print_space) SPRTF_STR(__str, logr_spc);
}

/**yihv
 * 键名
 * @param key
 */
#define hl_spc(str, spc)    sprtf_str(str, 0, spc)

#define hl_key_val(str, key, val)    sprtf_str(str, fetch_color(key, field_key_color), key)

/**
 * 键名
 * @param key
 */
#define hl_key(str, key)    sprtf_str(str, fetch_color(key, field_key_color), key)
#define HLSTR_KEY_P(strkey) STR_CPRINTF_P(strkey, C_CYAN)

/**
 * 等号
 * @param key
 */
#define hl_op(str, key)     sprtf_str(str, operator_color, key)

/**
 * 字符串
 * @param key
 */
#define hl_str(str, key)    sprtf_str(str, C_L_GRAY, key)
/**
 * 整型
 * @param key
 */
#define hl_long(str, key)   sprtf_long(str, C_L_GRAY, (long)key)
/**
 * 浮点型
 * @param key
 */
#define hl_double(str, key) sprtf_double(str, C_L_GRAY, (double)key)
/**
 * 时间
 * @param key
 */
#define hl_time(str, key) hl_str(str, key)
/**
 * 错误等级
 * @param key
 */
#define hl_level(str, key) hl_str(str, key)
/**
 * json NULL
 * @param key
 */
#define hl_jnul(str, key) hl_str(str, key)

/**
 * JSON 键名
 * @param key
 */
#define hl_jkey(str, key) hl_str(str, key)
/**
 * JSON 字符串
 * @param key
 */
#define hl_jstr(str, key) hl_str(str, key)
/**
 * JSON 整型
 * @param key
 */
#define hl_jint(str, key) hl_long(str, key)
/**
 * JSON 浮点型
 * @param key
 */
#define hl_jdbl(str, key) hl_double(str, key)
/**
 * JSON 布尔
 * @param key
 */
#define hl_jbln(str, key) hl_str(str, key)
/**
 * JSON 对象
 * @param key
 */
#define hl_jobj(str, key) hl_str(str, key)
/**
 * JSON 数组
 * @param key
 */
#define hl_jary(str, key) hl_str(str, key)
/**
 * JSON 引号
 * @param key
 */
#define hl_jqot(str, key) hl_str(str, key)
/**
 * JSON 冒号
 * @param key
 */
#define hl_jclo(str, key) hl_str(str, key)

/**
 * JSON 逗号
 * @param key
 */
#define hl_jcma(str, key) hl_str(str, key)


extern void parse_logr_colors (void);
extern void color_dict_free(void);

#endif //LOGR_HIGHLIGHT_H
