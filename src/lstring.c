//
// Created by Administrator on 2019/3/25.
//

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "lstring.h"
#include "utils.h"

/**
 * 对比两个字符串是否相等
 *
 * @param __s1
 * @param __s2
 * @param incase 是否区分大小写
 * @return
 */
int str_eq (const String *__s1, const String *__s2, const unsigned incase) {
    if (!__s1->str || !__s2->str) return 0;
    if (!__s1->len || !__s2->len) return 0;
    // 长度不同
    if (__s1->len != __s2->len) return 0;
    // 地址相同
    if (__s1->str == __s2->str) return 1;

    size_t len1 = __s1->len;
    size_t len2 = __s2->len;

    // 对比每一个字符
    if (incase) {
        while (len1 && len2 && tolower(*(__s1->str + (len1--))) != tolower(*(__s2->str + (len2--)))) return 0;
    } else {
        while (len1 && len2 && (*(__s1->str + (len1--))) != (*(__s2->str + (len2--)))) return 0;
    }

    return 1;
}


/**
 * 取子字符串，并支持删除两边空格
 *
 * @param str
 * @param len
 * @param trim 0保持原样；1删除空格
 * @return
 */
String* strsub(const char *str, size_t len, unsigned char trim) {
    size_t reallen;
    String *copy;

    if (len <= 0) return 0;

    reallen = strlen(str);
    if (reallen < len) {
        len = reallen;
    }

    if (trim) {
        while (*str == ' ') {
            str++;
            len--;
        }
        while (is_spcs(str + len - 1)) len--;
    }

    STR_INIT_P(copy);
    if (!copy) return 0;

    copy->len = len;
    copy->str = str;

    return copy;
}
