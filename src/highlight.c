//
// Created by Administrator on 2019/4/3.
//

#include <stdlib.h>
#include <ctype.h>
#include "highlight.h"
#include "utils.h"
#include "c-ctype.h"



/* 内置的基础颜色值变量 */
static char *field_key_color = C_CYAN;
static char *field_val_color = C_DARKGRAY;
static char *field_intval_color = NULL;
static char *field_floatval_color = NULL;
static char *field_numval_color = NULL;
static char *background_color = C_NONE;
static char *operator_color  = C_YELLOW;
static char *highlight_color  = C_RED;

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

/**
 * 支持自定义颜色输出日志
 *
 * @param __str
 * @param __key
 * @param __val
 * @param print_space
 */
bool sprtf_key_val (OutputBuffer*__str, const char *__key, const char *__val, const int opt) {
    /** 允许输出列名 */
    const bool _opt_pkey = (0 == (opt&OUTPUT_OPT_NOKEY));
    bool isPrinted = false;

    if (color_option) {
        const char *color = NULL;

        // 拼接全局的颜色处理
        if (NULL != __val) {
            snprintf(_strbuffer, MAX_BUFFER_SIZE, "*%s%s%s", __key, ",", __val);
            color = fetch_color(_strbuffer, NULL);
        }
        if (NULL == color) {
            snprintf(_strbuffer, MAX_BUFFER_SIZE, "*%s%s", __key, ",");
            color = fetch_color(_strbuffer, NULL);
        }

        if (NULL == color) {
            // 没有设置全局颜色
            if (_opt_pkey) {
                isPrinted = true;
                SPRTF_STR_OR_COLOR(__str, field_key_color, __key);
                SPRTF_STR_OR_COLOR(__str, operator_color, logr_op);
            }

            if (NULL != __val) {
                isPrinted = true;
                SPRTF_STR_OR_COLOR(__str, field_val_color, __val);
            }
        } else {
            if (_opt_pkey) {
                isPrinted = true;
                SPRTF_STR_COLOR(__str, color, __key);
                SPRTF_STR_COLOR(__str, color, logr_op);
            }
            if (NULL != __val) {
                isPrinted = true;
                SPRTF_STR_COLOR(__str, color, __val);
            }
        }
    } else {
        if (_opt_pkey) {
            isPrinted = true;
            SPRTF_STR(__str, __key);
            SPRTF_STR(__str, logr_op);
        }
        if (NULL != __val) {
            isPrinted = true;
            SPRTF_STR(__str, __val);
        }
    }

    // 输出分隔符
    if ((opt&OUTPUT_OPT_SEPARATOR)) SPRTF_STR(__str, logr_spc);

    return isPrinted;
}
/**
 * 输出高亮字符串颜色
 *
 * @param __str
 * @param __key
 * @param hl
 */
extern void sprtf_hl (OutputBuffer *__str, const char *__key, const Highlight *hl) {
    if (color_option) {
        if (highlight_color) {
            if (hl->pre)
                SPRTF_STR_CBUF(__str, field_val_color, hl->pre);
            SPRTF_STR_CBUF(__str, highlight_color, hl->str);
            SPRTF_STR_CBUF(__str, field_val_color, hl->app);
            return;
        }
    }
    SPRTF_STR(__str, __key);
}

static void color_cap_mt_fct(void) {
}

/* LOGR_COLORS.  */
static color_cap _color_dict_map[] =
        {
                {HL_INTERNAL_KEY,           &field_key_color,      color_cap_mt_fct},
                {HL_INTERNAL_VAL,           &field_val_color,      NULL},
                {HL_INTERNAL_BACKGROUND,    &background_color,     NULL},
                {HL_INTERNAL_OPERATOR,      &operator_color,       NULL},
                {HL_INTERNAL_HIGHLIGHT,     &highlight_color,      NULL},
                {NULL,                      NULL,                  NULL}
        };

static char *env_buffer;
void color_dict_free(void) {
    free(env_buffer);
}


/* TODO JSON高亮参数
   解析环境变量 LOGR_COLORS。默认值如下:
   LOGR_COLORS='k=96:v=90:*l,=97:*l,WARNING=93:*l,FATAL=91:*time=36:*time.ps_invoke=94:*errno,=91:*errno,0=90'
   颜色配置
内置：
	b(background，全局背景颜色)
	k(key，字段名的颜色)
	v(value，值的颜色)
	e(=，符号的颜色)
	hl(查找高亮的颜色)
	vn(值是数字类型的颜色)
	vf(值是浮点类型的颜色)
	vi(值是整数类型的颜色)
*开头：指定字段名称的颜色，使用英文句号分割json路径名。逗号分隔字段值，指定字段值的颜色。
  		例如：*l,warning=93  表示当日志错误等级(t)为warning时，将整个字段置为亮黄色。
*/
void parse_logr_colors (void) {
    const char *p;
    char *q, c;
    char *name;
    char *val;
    color_cap *cap;
    colors_map *cllet;

    p = getenv ("LOGR_COLORS");
    if (p == NULL || *p == '\0')
        p = HL_COLORS_DEFAULT;
    /* 创建一个可以修改的副本，无需释放内存  */
    env_buffer = q = strdup (p);

    name = q;
    val = NULL;

    for (;;) {
        c = *q;
        if (c == ':' || c == '\0') {

            *q++ = '\0'; /* Terminate name or val.  */
            /* 定义的 _color_dict_map 最后一个数组是空值
             * 这样如果未找到 name 将会不执行任何回调 */
            for (cap = _color_dict_map; cap->name; cap++)
                if (STREQ (cap->name, name))
                    break;
            /* 如果字段未找到，保存至全局变量 color_dict 中  */
            if (cap->var && val) {
                if (is_eof(val)) *(cap->var) = NULL;
                else {
                    *(cap->var) = val;
                }
            }
            if (cap->fct)
                cap->fct();
            if (NULL == cap->name && colors_dict_len < MAX_COLORS) {
                cllet = &colors_dict[colors_dict_len++];
                cllet->name = (name);
                cllet->var = (val);
            }
            if (c == '\0')
                break;
            name = q;
            val = NULL;
        } else if (c == '=') {
            if (q == name || val)
                break;
            *q++ = '\0'; /* 字段名称已结束.  */
            val = q; /* 保证 颜色值 可为空值  */
        } else if (val == NULL)
            q++; /* 向后读取字段名称  */
        else if (c == ';' || c_isdigit(c))
            q++; /* 向后读取 颜色值  */
        else
            break;
    }
}


void remove_color(char *line) {
    char *str = line;
    int jump;
    while(!is_eof(str)) {
        jump = 1;
        if (*str == 27 && *(str+1) == '[') {
            jump = 2;
            if (*(str+jump) >= '0' && *(str+jump) <= '9') {
                jump ++;
            }
            if (*(str+jump) >= '0' && *(str+jump) <= '9') {
                jump ++;
            }
            if (*(str+jump) == ';') {
                jump++;
                if (*(str+jump) >= '0' && *(str+jump) <= '9') {
                    jump ++;
                }
                if (*(str+jump) >= '0' && *(str+jump) <= '9') {
                    jump ++;
                }
            }
            if (*(str+jump) == 'm' || *(str+jump) == 'G' || *(str+jump) == 'K') {
                jump++;
            }
        }
        if (jump > 1) {
            str += jump;
            continue;
        }
        *(line++) = *(str++);
    }
    *line = '\0';
}