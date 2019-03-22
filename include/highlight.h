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
static void hl_key(const char *key){
    printf(C_CYAN"%s"C_NONE, key);
}
/**
 * 等号
 * @param key
 */
static void hl_op(const char *key){
    printf(C_BROWN"%s"C_NONE, key);
}

/**
 * 字符串
 * @param key
 */
static void hl_str(const char *key){
    printf(C_L_GRAY"%s"C_NONE, key);
}
/**
 * 整型
 * @param key
 */
static void hl_long(const long key){
    printf(C_L_GRAY"%ld"C_NONE, key);
}
/**
 * 浮点型
 * @param key
 */
static void hl_double(const double key){
    printf(C_L_GRAY"%lf"C_NONE, key);
}
/**
 * 时间
 * @param key
 */
static void hl_time(const char *key){
    hl_str(key);
}
/**
 * 错误等级
 * @param key
 */
static void hl_level(const char *key){
    hl_str(key);
}
/**
 * json
 * @param key
 */
static void hl_json(const char *key){
    hl_str(key);
}

/**
 * JSON 键名
 * @param key
 */
static void hl_jkey(const char *key){
    hl_str(key);
}
/**
 * JSON 字符串
 * @param key
 */
static void hl_jstr(const char *key){
    hl_str(key);
}
/**
 * JSON 整型
 * @param key
 */
static void hl_jint(const char *key){
    hl_str(key);
}
/**
 * JSON 布尔
 * @param key
 */
static void hl_jbln(const char *key){
    hl_str(key);
}
/**
 * JSON 对象
 * @param key
 */
static void hl_jobj(const char *key){
    hl_str(key);
}
/**
 * JSON 数组
 * @param key
 */
static void hl_jary(const char *key){
    hl_str(key);
}
/**
 * JSON 引号
 * @param key
 */
static void hl_jqot(const char *key){
    hl_str(key);
}
/**
 * JSON 冒号
 * @param key
 */
static void hl_jclo(const char *key){
    hl_str(key);
}

/**
 * JSON 逗号
 * @param key
 */
static void hl_jcma(const char *key){
    hl_str(key);
}
#endif //LOGR_HIGHLIGHT_H
