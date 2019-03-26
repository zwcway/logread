//
// Created by Administrator on 2019/3/13.
//

#ifndef LOGR_HIGHLIGHT_H
#define LOGR_HIGHLIGHT_H

#include "color.h"
#include "format.h"

/**
 * 键名
 * @param key
 */
#define hl_key(key) printf(C_CYAN"%s"C_NONE, key)

/**
 * 等号
 * @param key
 */
#define hl_op(key)  printf(C_BROWN"%s"C_NONE, key)

/**
 * 字符串
 * @param key
 */
#define hl_str(key) printf(C_L_GRAY"%s"C_NONE, key)
/**
 * 整型
 * @param key
 */
#define hl_long(key) printf(C_L_GRAY"%ld"C_NONE, (long)key)
/**
 * 浮点型
 * @param key
 */
#define hl_double(key) printf(C_L_GRAY"%lf"C_NONE, key)
/**
 * 时间
 * @param key
 */
#define hl_time(key) hl_str(key)
/**
 * 错误等级
 * @param key
 */
#define hl_level(key) hl_str(key)
/**
 * json NULL
 * @param key
 */
#define hl_jnul(key) hl_str(key)

/**
 * JSON 键名
 * @param key
 */
#define hl_jkey(key) hl_str(key)
/**
 * JSON 字符串
 * @param key
 */
#define hl_jstr(key) hl_str(key)
/**
 * JSON 整型
 * @param key
 */
#define hl_jint(key) hl_long(key)
/**
 * JSON 浮点型
 * @param key
 */
#define hl_jdbl(key) hl_double(key)
/**
 * JSON 布尔
 * @param key
 */
#define hl_jbln(key) hl_str(key)
/**
 * JSON 对象
 * @param key
 */
#define hl_jobj(key) hl_str(key)
/**
 * JSON 数组
 * @param key
 */
#define hl_jary(key) hl_str(key)
/**
 * JSON 引号
 * @param key
 */
#define hl_jqot(key) hl_str(key)
/**
 * JSON 冒号
 * @param key
 */
#define hl_jclo(key) hl_str(key)

/**
 * JSON 逗号
 * @param key
 */
#define hl_jcma(key) hl_str(key)

#endif //LOGR_HIGHLIGHT_H
