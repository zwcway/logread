//
// Created by Administrator on 2019/3/13.
//

#ifndef LOGR_HIGHLIGHT_H
#define LOGR_HIGHLIGHT_H

#include "color.h"
#include "format.h"
#include "lstring.h"
#include "logr.h"

#define sprtf(str, fmt, arg) (*(str)) += snprintf(*(str), MAX_LINE - (outputstr - *(str)), fmt, arg)

/**yihv
 * 键名
 * @param key
 */
#define hl_spc(str, spc)    sprtf(str, "%s", spc)

/**
 * 键名
 * @param key
 */
#define hl_key(str, key)    sprtf(str, C_CYAN"%s"C_NONE, key)
#define HLSTR_KEY_P(strkey) STR_CPRINTF_P(strkey, C_CYAN)

/**
 * 等号
 * @param key
 */
#define hl_op(str, key)     sprtf(str, C_BROWN"%s"C_NONE, key)

/**
 * 字符串
 * @param key
 */
#define hl_str(str, key)    sprtf(str, C_L_GRAY"%s"C_NONE, key)
/**
 * 整型
 * @param key
 */
#define hl_long(str, key)   sprtf(str, C_L_GRAY"%ld"C_NONE, (long)key)
/**
 * 浮点型
 * @param key
 */
#define hl_double(str, key) sprtf(str, C_L_GRAY"%lf"C_NONE, (double)key)
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

#endif //LOGR_HIGHLIGHT_H
