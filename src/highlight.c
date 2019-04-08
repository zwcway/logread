//
// Created by Administrator on 2019/4/3.
//

#include <stdlib.h>
#include <ctype.h>
#include "highlight.h"
#include "utils.h"
#include "c-ctype.h"

static void color_cap_mt_fct(void) {
}

/* LOGR_COLORS.  */
static const color_cap _color_dict_map[] =
        {
                {HL_INTERNAL_KEY,           &field_key_color,      color_cap_mt_fct},
                {HL_INTERNAL_VAL,           &field_val_color,      NULL},
                {HL_INTERNAL_BACKGROUND,    &background_color,     NULL},
                {HL_INTERNAL_OPERATOR,      &operator_color,       NULL},
                {NULL,                      NULL,                  NULL}
        };

void color_dict_free(void) {
    colors_map *cllet;
    for (cllet = colors_dict; cllet->name; cllet++) {
        if (cllet->var) free(cllet->var);
        if (cllet->name) free(cllet->name);
    }
}


/* 解析环境变量 LOGR_COLORS。默认值如下:
   LOGR_COLORS='ky=96:vl=90:*l,=97:*l,WARNING=93:*l,FATAL=91:*time=36:*time.ps_invoke=94:*errno,=91:*errno,0=90'
   颜色配置
内置：
	bg(background，全局背景颜色)
	ky(key，字段名的颜色)
	vl(value，值的颜色)
	eq(=，符号的颜色)
*开头：指定字段名称的颜色，使用英文句号分割json路径名。逗号分隔字段值，指定字段值的颜色。
  		例如：*l,warning=93  表示当日志错误等级(t)为warning时，将整个字段置为亮黄色。
*/
void parse_logr_colors (void) {
    const char *p;
    char *q, c, *qpoint;
    char *name;
    char *val;
    color_cap const *cap;
    colors_map *cllet;

    p = getenv ("LOGR_COLORS");
    if (p == NULL || *p == '\0')
        return;

    /* 创建一个可以修改的副本，无需释放内存  */
    qpoint = q = strdup (p);

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
            if (cap->var && val)
                *(cap->var) = (val);
            if (cap->fct)
                cap->fct();
            if (NULL == cap->name && colors_dict_len < MAX_COLORS) {
                cllet = &colors_dict[colors_dict_len++];
                cllet->name = strdup(name);
                cllet->var = strdup(val);
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

    free(qpoint);
}
