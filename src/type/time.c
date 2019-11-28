//
// Created by Administrator on 2019/11/28.
//

/**
 * 在 tm 这个结构中, 只有年月日时分秒6个必要的元素, 其它的数字可以计算得到.
 * 因为结构定义年从1900年开始, 而转换后的统一秒数只能达到136年, 所以目前能够识别
 * 的年限范围是在1900~2036年之间.
 * 本文件提供的字符串转日期目前只支持常见的中英文格式, 比如年月日, 时分秒,
 * 通常人能够直观读出的日期读数, 本文应该都可以识别, 比如:
 *  sep 3, 1995, 20:30      -> 1995-09-03 20:30:00
 *  3sep97                  -> 1997-09-03 00:00:00
 *  oct 4,03                -> 2003-10-04 00:00:00
 *  mon dec 29, 2008        -> 2008-12-29 00:00:00
 *  2011 nov 11, 11:11:11   -> 2011-11-11 11:11:11
 *  10:00am, 2008/9/1       -> 2008-09-01 10:00:00
 *  10/01/1976 3:01:05pm    -> 1976-10-01 15:01:05
 * 但产生歧议的日期, 比如 01/02/03, 本文将按照月份优先, 日期其次,
 * 最后是年份的格式将被解读成 2003年1月2日.
 *
 * 本文亦可识别一连串的数字组成的日期格式, 但月日时分秒必须使用两位十进制数表示.
 *
 */

#include "time.h"

/**
 * 找到下一个时间标记
 *
 * 内部函数, 要求入参指针必须非空
 *
 * @param timeStr    - 时间日期字符串
 * @param pVal       - 标记可以转换的年月日及时间的读数值
 * @param pFlag      - 类型标记, 标明解析的是数字, 还是有量纲的值
 * @return 解析过的字符长度, 失败时返回0
 */
static unsigned long nextTimeToken(char *timeStr, int *pVal, int *pFlag) {
    char *pChar = timeStr, *pCharEnd;
    int iFlags = 0, iTemp;

    *pFlag = TOKEN_INIT;

    /* 跳过分隔符 */
    while (IS_TIME_DELIM(*pChar) || IS_DATE_DELIM(*pChar)) {
        pChar++;
    }

    /* 是数值串, 直接返回数值 */
    if (isdigit(*pChar)) {
        for (*pVal = 0, pCharEnd = pChar; isdigit(*pCharEnd); pCharEnd++) {
            *pVal = *pVal * 10 + (*pCharEnd - '0');
        }

        /* 英文字母序号, 被指定为day */
        if (*(pCharEnd - 1) == '1' && strncmp(pCharEnd, "st", 2) == 0) {
            *pFlag = TOKEN_DAY, pCharEnd += 2;
        } else if (*(pCharEnd - 1) == '2' && strncmp(pCharEnd, "nd", 2) == 0) {
            *pFlag = TOKEN_DAY, pCharEnd += 2;
        } else if (*(pCharEnd - 1) == '3' && strncmp(pCharEnd, "rd", 2) == 0) {
            *pFlag = TOKEN_DAY, pCharEnd += 2;
        } else if (strncmp(pCharEnd, "th", 2) == 0) {
            *pFlag = TOKEN_DAY, pCharEnd += 2;
        }

        if (*pFlag == TOKEN_DAY) {
            return (pCharEnd - timeStr);
        }

#if 1   /* 为中文服务, 可忽略 */
        /* 跳过空格, 以检查单位识别符 */
        while (isspace(*pChar)) {
            pChar++;
        }
        /* 汉字量纲, 被特许为可识别的日期标识 */
        if (strncmp(pCharEnd, "年", 2) == 0) {
            *pFlag = TOKEN_YEAR, pCharEnd += 2;
        } else if (strncmp(pCharEnd, "月", 2) == 0) {
            *pFlag = TOKEN_MONTH, pCharEnd += 2;
        } else if (strncmp(pCharEnd, "日", 2) == 0) {
            *pFlag = TOKEN_DAY, pCharEnd += 2;
        } else if (strncmp(pCharEnd, "时", 2) == 0) {
            *pFlag = TOKEN_HOUR, pCharEnd += 2;
        } else if (strncmp(pCharEnd, "分", 2) == 0) {
            *pFlag = TOKEN_MIN, pCharEnd += 2;
        } else if (strncmp(pCharEnd, "秒", 2) == 0) {
            *pFlag = TOKEN_SEC, pCharEnd += 2;
        } else
#endif
        {
            *pFlag = TOKEN_VAL;
        }
        return pCharEnd - timeStr;

    }

    /* 不是数值且不是字母(其它标点, 不可显示字符), 一般不被支持, 返回0 */
    if (!isalpha(*pChar)) {
        if (*pChar >= 0) {
            return 0;
        }
        /* 非ASCII字符集, 比如是中文"星期一"之类, 跳过 */
        pCharEnd = pChar;
        while (*pCharEnd != ' ' && *pCharEnd != ',' && *pCharEnd != '\0') {
            pCharEnd++;
        }
        return pCharEnd - timeStr;
    }

    /* 是字母, 检查是不是月份 */
    for (iTemp = 0; iTemp < 12; iTemp++) {
        if (strncmp(pChar, s_cMonth + iTemp * 3, 3) == 0) {
            *pVal = iTemp + 1;
            break;
        } else if (strncmp(pChar, s_cMonthL + iTemp * 3, 3) == 0) {
            *pVal = iTemp + 1;
            break;
        } else if (strncmp(pChar, s_cMonthU + iTemp * 3, 3) == 0) {
            *pVal = iTemp + 1;
            break;
        }
    } /* 月份字母检查 */

    /* 找到月份字母, 跳过尾部字母 */
    if (iTemp < 12) {
        pCharEnd = pChar + 3;
        while (isalpha(*pCharEnd)) {
            pCharEnd++;
        }
        *pFlag = TOKEN_MONTH;
        return pCharEnd - timeStr;
    }

    /* 未找到月份字母, 则检查星期字母 */
    for (iTemp = 0; iTemp < 7; iTemp++) {
        if (strncmp(pChar, s_cWeekDay + iTemp * 3, 3) == 0) {
            *pVal = iTemp;
            break;
        } else if (strncmp(pChar, s_cWeekDayL + iTemp * 3, 3) == 0) {
            *pVal = iTemp;
            break;
        } else if (strncmp(pChar, s_cWeekDayU + iTemp * 3, 3) == 0) {
            *pVal = iTemp;
            break;
        }
    } /* 星期字母检查 */

    /* 找到星期字母 */
    if (iTemp < 7) {
        pCharEnd = pChar + 3;
        while (isalpha(*pCharEnd)) {
            pCharEnd++;
        }
        *pFlag = TOKEN_WEEKDAY;
        return pCharEnd - timeStr;
    }

    /* 午前午后标志检查 */
    if (strncmp(pChar, "am", 2) == 0 || strncmp(pChar, "AM", 2) == 0) {
        pCharEnd = pChar + 2;
        if (!isalpha(*pCharEnd)) {
            *pFlag = TOKEN_HALFDAY;
            *pVal = 0;
            return pCharEnd - timeStr;
        }
    }
    if (strncmp(pChar, "pm", 2) == 0 || strncmp(pChar, "PM", 2) == 0) {
        pCharEnd = pChar + 2;
        if (!isalpha(*pCharEnd)) {
            *pFlag = TOKEN_HALFDAY;
            *pVal = 1;
            return pCharEnd - timeStr;
        }
    }

    return 0;

}

/**
 * 将日期数字转换为时间结构, 必须按年月日时分秒排列, 否则看不懂
 *
 * 歧义和冲突处理, 按排列的优先顺序进行
 *
 * @param timeStr    - 时间日期字符串
 * @param iLen       - 时间日期字符串长度
 * @param ptTime     - 日期时间结构
 * @return 年月日时分秒有效的标志
 */
static int digit2time(char *timeStr, unsigned long iLen, Time *ptTime) {
    char *pChar = timeStr;

    memset(ptTime, 0, sizeof(*ptTime));

    if (iLen < 6 || iLen % 2 != 0) {
        return 0;
    }

    /* 允许的日期时间格式(按常用频度排列): 5 + 4 + 3 + 2 + 1 = 15
        纪年月日时分秒  - YYYYMMDDHHmmss
        年月日时分秒    - YYMMDDHHmmss
        纪年月日时分    - YYYYMMDDHHmm
        月日时分秒      - MMDDHHmmss
        年月日时分      - YYMMDDHHmm
        纪年月日时      - YYYYMMDDHH
        月日时分        - MMDDHHmm
        纪年月日        - YYYYMMDD
        日时分秒        - DDHHmmss
        年月日时        - YYMMDDHH
        年月日          - YYMMDD
        纪年月          - YYYYMM
        时分秒          - HHmmss
        日时分          - DDHHmm
        月日时          - MMDDHH
    */

    /* 超长格式, 只有: YYYYMMDDHHmmss */
    if (iLen >= 14) {
        ptTime->tm.tm_year = (pChar[0] - '0') * 1000 + (pChar[1] - '0') * 100 +
                          (pChar[2] - '0') * 10 + (pChar[3] - '0');
        ptTime->tm.tm_mon = (pChar[4] - '0') * 10 + (pChar[5] - '0');
        ptTime->tm.tm_mday = (pChar[6] - '0') * 10 + (pChar[7] - '0');
        ptTime->tm.tm_hour = (pChar[8] - '0') * 10 + (pChar[9] - '0');
        ptTime->tm.tm_min = (pChar[10] - '0') * 10 + (pChar[11] - '0');
        ptTime->tm.tm_sec = (pChar[12] - '0') * 10 + (pChar[13] - '0');
        if (ptTime->tm.tm_year >= 1900 && ptTime->tm.tm_year < 2036 &&
            ptTime->tm.tm_mon < 13 &&
            ptTime->tm.tm_mday <= 29 + s_iMonthFix[ptTime->tm.tm_mon - 1] &&
            ptTime->tm.tm_hour < 24 && ptTime->tm.tm_min < 60 && ptTime->tm.tm_sec < 60) {
            return FLAG_SEC | FLAG_MIN | FLAG_HOUR |
                   FLAG_DAY | FLAG_MONTH | FLAG_YEAR;
        }
    }
        /* 次长格式, 有: YYYYMMDDHHmm, YYMMDDHHmmss */
    else if (iLen == 12) {
        /* YYYYMMDDHHmm */
        ptTime->tm.tm_year = (pChar[0] - '0') * 1000 + (pChar[1] - '0') * 100 +
                          (pChar[2] - '0') * 10 + (pChar[3] - '0');
        ptTime->tm.tm_mon = (pChar[4] - '0') * 10 + (pChar[5] - '0');
        ptTime->tm.tm_mday = (pChar[6] - '0') * 10 + (pChar[7] - '0');
        ptTime->tm.tm_hour = (pChar[8] - '0') * 10 + (pChar[9] - '0');
        ptTime->tm.tm_min = (pChar[10] - '0') * 10 + (pChar[11] - '0');
        if (ptTime->tm.tm_year >= 1900 && ptTime->tm.tm_year < 2036 &&
            ptTime->tm.tm_mon < 13 &&
            ptTime->tm.tm_mday <= 29 + s_iMonthFix[ptTime->tm.tm_mon - 1] &&
            ptTime->tm.tm_hour < 24 && ptTime->tm.tm_min < 60) {
            return FLAG_MIN | FLAG_HOUR | FLAG_DAY | FLAG_MONTH | FLAG_YEAR;
        }
        /* YYMMDDHHmmss */
        ptTime->tm.tm_year = (pChar[0] - '0') * 10 + (pChar[1] - '0');
        ptTime->tm.tm_year += (ptTime->tm.tm_year >= 70) ? 1900 : 2000;
        ptTime->tm.tm_mon = (pChar[2] - '0') * 10 + (pChar[3] - '0');
        ptTime->tm.tm_mday = (pChar[4] - '0') * 10 + (pChar[5] - '0');
        ptTime->tm.tm_hour = (pChar[6] - '0') * 10 + (pChar[7] - '0');
        ptTime->tm.tm_min = (pChar[8] - '0') * 10 + (pChar[9] - '0');
        ptTime->tm.tm_sec = (pChar[10] - '0') * 10 + (pChar[11] - '0');
        if (ptTime->tm.tm_year >= 1900 && ptTime->tm.tm_year < 2036 &&
            ptTime->tm.tm_mon < 13 &&
            ptTime->tm.tm_mday <= 29 + s_iMonthFix[ptTime->tm.tm_mon - 1] &&
            ptTime->tm.tm_hour < 24 && ptTime->tm.tm_min < 60 && ptTime->tm.tm_sec < 60) {
            return FLAG_SEC | FLAG_MIN | FLAG_HOUR |
                   FLAG_DAY | FLAG_MONTH | FLAG_YEAR;
        }
    }
        /* 10字节, 有: MMDDHHmmss, YYMMDDHHmm, YYYYMMDDHH */
    else if (iLen == 10) {
        /* MMDDHHmmss */
        ptTime->tm.tm_mon = (pChar[0] - '0') * 10 + (pChar[1] - '0');
        ptTime->tm.tm_mday = (pChar[2] - '0') * 10 + (pChar[3] - '0');
        ptTime->tm.tm_hour = (pChar[4] - '0') * 10 + (pChar[5] - '0');
        ptTime->tm.tm_min = (pChar[6] - '0') * 10 + (pChar[7] - '0');
        ptTime->tm.tm_sec = (pChar[8] - '0') * 10 + (pChar[9] - '0');
        if (ptTime->tm.tm_mon < 13 &&
            ptTime->tm.tm_mday <= 29 + s_iMonthFix[ptTime->tm.tm_mon - 1] &&
            ptTime->tm.tm_hour < 24 && ptTime->tm.tm_min < 60 && ptTime->tm.tm_sec < 60) {
            return FLAG_SEC | FLAG_MIN | FLAG_HOUR | FLAG_DAY | FLAG_MONTH;
        }
        /* YYMMDDHHmm */
        ptTime->tm.tm_year = (pChar[0] - '0') * 10 + (pChar[1] - '0');
        ptTime->tm.tm_year += (ptTime->tm.tm_year >= 70) ? 1900 : 2000;
        ptTime->tm.tm_mon = (pChar[2] - '0') * 10 + (pChar[3] - '0');
        ptTime->tm.tm_mday = (pChar[4] - '0') * 10 + (pChar[5] - '0');
        ptTime->tm.tm_hour = (pChar[6] - '0') * 10 + (pChar[7] - '0');
        ptTime->tm.tm_min = (pChar[8] - '0') * 10 + (pChar[9] - '0');
        if (ptTime->tm.tm_year >= 1900 && ptTime->tm.tm_year < 2036 &&
            ptTime->tm.tm_mon < 13 &&
            ptTime->tm.tm_mday <= 29 + s_iMonthFix[ptTime->tm.tm_mon - 1] &&
            ptTime->tm.tm_hour < 24 && ptTime->tm.tm_min < 60) {
            return FLAG_MIN | FLAG_HOUR | FLAG_DAY | FLAG_MONTH | FLAG_YEAR;
        }
        /* YYYYMMDDHH */
        ptTime->tm.tm_year = (pChar[0] - '0') * 1000 + (pChar[1] - '0') * 100 +
                          (pChar[2] - '0') * 10 + (pChar[3] - '0');
        ptTime->tm.tm_mon = (pChar[4] - '0') * 10 + (pChar[5] - '0');
        ptTime->tm.tm_mday = (pChar[6] - '0') * 10 + (pChar[7] - '0');
        ptTime->tm.tm_hour = (pChar[8] - '0') * 10 + (pChar[9] - '0');
        if (ptTime->tm.tm_hour < 24 &&
            ptTime->tm.tm_year >= 1900 && ptTime->tm.tm_year < 2036 &&
            ptTime->tm.tm_mon < 13 &&
            ptTime->tm.tm_mday <= 29 + s_iMonthFix[ptTime->tm.tm_mon - 1]) {
            return FLAG_HOUR | FLAG_DAY | FLAG_MONTH | FLAG_YEAR;
        }
    }
        /* 8字节, 有: MMDDHHmm, YYYYMMDD, DDHHmmss, YYMMDDHH */
    else if (iLen == 8) {
        /* MMDDHHmm */
        ptTime->tm.tm_mon = (pChar[0] - '0') * 10 + (pChar[1] - '0');
        ptTime->tm.tm_mday = (pChar[2] - '0') * 10 + (pChar[3] - '0');
        ptTime->tm.tm_hour = (pChar[4] - '0') * 10 + (pChar[5] - '0');
        ptTime->tm.tm_min = (pChar[6] - '0') * 10 + (pChar[7] - '0');
        if (ptTime->tm.tm_mon < 13 &&
            ptTime->tm.tm_mday <= 29 + s_iMonthFix[ptTime->tm.tm_mon - 1] &&
            ptTime->tm.tm_hour < 24 && ptTime->tm.tm_min < 60) {
            return FLAG_MIN | FLAG_HOUR | FLAG_DAY | FLAG_MONTH;
        }
        /* YYYYMMDD */
        ptTime->tm.tm_year = (pChar[0] - '0') * 1000 + (pChar[1] - '0') * 100 +
                          (pChar[2] - '0') * 10 + (pChar[3] - '0');
        ptTime->tm.tm_mon = (pChar[4] - '0') * 10 + (pChar[5] - '0');
        ptTime->tm.tm_mday = (pChar[6] - '0') * 10 + (pChar[7] - '0');
        if (ptTime->tm.tm_year >= 1900 && ptTime->tm.tm_year < 2036 &&
            ptTime->tm.tm_mon < 13 &&
            ptTime->tm.tm_mday <= 29 + s_iMonthFix[ptTime->tm.tm_mon - 1]) {
            return FLAG_DAY | FLAG_MONTH | FLAG_YEAR;
        }
        /* DDHHmmss */
        ptTime->tm.tm_mday = (pChar[0] - '0') * 10 + (pChar[1] - '0');
        ptTime->tm.tm_hour = (pChar[2] - '0') * 10 + (pChar[3] - '0');
        ptTime->tm.tm_min = (pChar[4] - '0') * 10 + (pChar[5] - '0');
        ptTime->tm.tm_sec = (pChar[6] - '0') * 10 + (pChar[7] - '0');
        if (ptTime->tm.tm_mday <= 31 && ptTime->tm.tm_hour < 24 &&
            ptTime->tm.tm_min < 60 && ptTime->tm.tm_sec < 60) {
            return FLAG_SEC | FLAG_MIN | FLAG_HOUR | FLAG_DAY;
        }
        /* YYMMDDHH */
        ptTime->tm.tm_year = (pChar[0] - '0') * 10 + (pChar[1] - '0');
        ptTime->tm.tm_year += (ptTime->tm.tm_year >= 70) ? 1900 : 2000;
        ptTime->tm.tm_mon = (pChar[2] - '0') * 10 + (pChar[3] - '0');
        ptTime->tm.tm_mday = (pChar[4] - '0') * 10 + (pChar[5] - '0');
        ptTime->tm.tm_hour = (pChar[6] - '0') * 10 + (pChar[7] - '0');
        if (ptTime->tm.tm_hour < 24 &&
            ptTime->tm.tm_year >= 1900 && ptTime->tm.tm_year < 2036 &&
            ptTime->tm.tm_mon < 13 &&
            ptTime->tm.tm_mday <= 29 + s_iMonthFix[ptTime->tm.tm_mon - 1]) {
            return FLAG_HOUR | FLAG_DAY | FLAG_MONTH | FLAG_YEAR;
        }
    } else {
        /* YYMMDD */
        ptTime->tm.tm_year = (pChar[0] - '0') * 10 + (pChar[1] - '0');
        ptTime->tm.tm_year += (ptTime->tm.tm_year >= 70) ? 1900 : 2000;
        ptTime->tm.tm_mon = (pChar[2] - '0') * 10 + (pChar[3] - '0');
        ptTime->tm.tm_mday = (pChar[4] - '0') * 10 + (pChar[5] - '0');
        if (ptTime->tm.tm_year >= 1900 && ptTime->tm.tm_year < 2036 &&
            ptTime->tm.tm_mon < 13 &&
            ptTime->tm.tm_mday <= 29 + s_iMonthFix[ptTime->tm.tm_mon - 1]) {
            return FLAG_DAY | FLAG_MONTH | FLAG_YEAR;
        }
        /* YYYYMM */
        ptTime->tm.tm_year = (pChar[0] - '0') * 1000 + (pChar[1] - '0') * 100 +
                          (pChar[2] - '0') * 10 + (pChar[3] - '0');
        ptTime->tm.tm_mon = (pChar[4] - '0') * 10 + (pChar[5] - '0');
        if (ptTime->tm.tm_year >= 1900 && ptTime->tm.tm_year < 2036 &&
            ptTime->tm.tm_mon < 13) {
            return FLAG_MONTH | FLAG_YEAR;
        }
        /* HHmmss */
        ptTime->tm.tm_hour = (pChar[0] - '0') * 10 + (pChar[1] - '0');
        ptTime->tm.tm_min = (pChar[2] - '0') * 10 + (pChar[3] - '0');
        ptTime->tm.tm_sec = (pChar[4] - '0') * 10 + (pChar[5] - '0');
        if (ptTime->tm.tm_hour < 24 && ptTime->tm.tm_min < 60 && ptTime->tm.tm_sec < 60) {
            return FLAG_SEC | FLAG_MIN | FLAG_HOUR;
        }
        /* DDHHmm */
        ptTime->tm.tm_mday = (pChar[0] - '0') * 10 + (pChar[1] - '0');
        ptTime->tm.tm_hour = (pChar[2] - '0') * 10 + (pChar[3] - '0');
        ptTime->tm.tm_min = (pChar[4] - '0') * 10 + (pChar[5] - '0');
        if (ptTime->tm.tm_mday <= 31 && ptTime->tm.tm_hour < 24 && ptTime->tm.tm_min < 60) {
            return FLAG_MIN | FLAG_HOUR | FLAG_DAY;
        }
        /* MMDDHH */
        ptTime->tm.tm_mon = (pChar[0] - '0') * 10 + (pChar[1] - '0');
        ptTime->tm.tm_mday = (pChar[2] - '0') * 10 + (pChar[3] - '0');
        ptTime->tm.tm_hour = (pChar[4] - '0') * 10 + (pChar[5] - '0');
        if (ptTime->tm.tm_mon < 13 &&
            ptTime->tm.tm_mday <= 29 + s_iMonthFix[ptTime->tm.tm_mon - 1] &&
            ptTime->tm.tm_hour < 24) {
            return FLAG_HOUR | FLAG_DAY | FLAG_MONTH;
        }
    }

    return 0;
}

/**
 * 将日期及(或)时间字符串转换为时间结构, 例: Dec 19 2008, 13:35:40
 *
 * 时分秒之间必须使用冒号":"分隔, 并且必然是此顺序
 *
 * @param timeStr    - 时间日期字符串
 * @param ptTime     - 日期时间结构
 * @return 年月日时分秒有效的标志
 */
int strtotime(const char *timeStr, Time *ptTime) {
    int iFlags, iVal, iStep, iDateTimeFlag;
    unsigned long iLen;
    int iYear, iMonth, iDay, iHour, iMinute, iSecond;
    char *pChar = (char*)timeStr, *pCharEnd = (char*)timeStr, tempStr[8];

    iDateTimeFlag = iYear = iMonth = iDay = iHour = iMinute = iSecond = 0;
    memset(ptTime, 0, sizeof(*ptTime));

    /* 跳过前导空格 */
    while (*pChar == ' ' || *pChar == '\t') {
        pChar++;
    }

    /* 不存在可解析的中间字符串 */
    if ((iLen = nextTimeToken(pChar, &iVal, &iFlags)) <= 0) {
        return iDateTimeFlag;
    }

    /* 数值串且长度大于等于6, 则为全数字型日期时间, 如20081229080306
     * 需要重新解析 */
    if (iFlags == TOKEN_VAL && iLen >= 6 && iLen % 2 == 0) {
        iDateTimeFlag = digit2time(pChar, iLen, ptTime);
        if (iDateTimeFlag == 0) {
            memset(ptTime, 0, sizeof(*ptTime));
        }
        /* 由于进行了值尝试, 当不适用时, 要清除尝试值 */
        ptTime->tm.tm_year = (iDateTimeFlag & FLAG_YEAR) ? ptTime->tm.tm_year - 1900 : 0;

        ptTime->tm.tm_mon = (iDateTimeFlag & FLAG_MONTH) ? ptTime->tm.tm_mon - 1 : 0;

        if ((iDateTimeFlag & FLAG_DAY) == 0) {
            ptTime->tm.tm_mday = 1;
        }
        if ((iDateTimeFlag & FLAG_HOUR) == 0) {
            ptTime->tm.tm_hour = 0;
        }
        if ((iDateTimeFlag & FLAG_MIN) == 0) {
            ptTime->tm.tm_min = 0;
        }
        if ((iDateTimeFlag & FLAG_SEC) == 0) {
            ptTime->tm.tm_sec = 0;
        }
        if (!iDateTimeFlag) {
            goto strToTimeErrOut;
        }
        ptTime->ts = mktime(&ptTime->tm);
        return iDateTimeFlag;
    }

    /* 否则各字段之间必然有分隔符(或数字英文分隔, 比如 22sep92) */
    for (iStep = STEP_INIT; *pChar != '\0' && iLen > 0; pChar += iLen, iLen = nextTimeToken(pChar, &iVal, &iFlags)) {
        /* 强格式年份, 直接填充到年份信息中 */
        if (iFlags == TOKEN_YEAR) {
            if ((iDateTimeFlag & FLAG_YEAR) == 0) {
                iYear = iVal, iDateTimeFlag |= FLAG_YEAR;
            } else {
                goto strToTimeErrOut;
            }
            iStep = STEP_DATE, iDateTimeFlag |= FLAG_DATE;
            continue;
        }
            /* 强格式月份, 直接填充到月份信息中(否则需要试探比较) */
        else if (iFlags == TOKEN_MONTH) {
            if ((iDateTimeFlag & FLAG_MONTH) == 0) {
                iMonth = iVal, iDateTimeFlag |= FLAG_ALPHA | FLAG_MONTH;
            }
                /* 有月份信息但非字母指定, 说明是试探, 则将其移走(值必居于1~12) */
            else if ((iDateTimeFlag & FLAG_ALPHA) == 0) {
                if ((iDateTimeFlag & FLAG_DAY) == 0) {
                    iDay = iMonth, iDateTimeFlag |= FLAG_DAY;
                } else if ((iDateTimeFlag & FLAG_YEAR) == 0) /* 2001 ~ 2012年 */
                {
                    iYear = iMonth, iDateTimeFlag |= FLAG_YEAR;
                } else {
                    goto strToTimeErrOut;
                }
                iMonth = iVal, iDateTimeFlag |= FLAG_ALPHA;
            } else {
                goto strToTimeErrOut;
            }
            iStep = STEP_DATE, iDateTimeFlag |= FLAG_DATE;
            continue;
        } /* (iFlags == TOKEN_MONTH) */
            /* 强格式日期, 直接填充到日期信息中 */
        else if (iFlags == TOKEN_DAY) {
            if ((iDateTimeFlag & FLAG_DAY) == 0) {
                iDay = iVal, iDateTimeFlag |= FLAG_DAY;
            } else {
                goto strToTimeErrOut;
            }
            iStep = STEP_DATE, iDateTimeFlag |= FLAG_DATE;
            continue;
        }
            /* 半日信息(上午, 下午, 必然在时间后, 不然不予认可) */
        else if (iFlags == TOKEN_HALFDAY) {
            if ((iDateTimeFlag & FLAG_HALFDAY) == 0 &&
                iHour + iVal * 12 < 24) {
                iHour += iVal * 12, iDateTimeFlag |= FLAG_HALFDAY;
            } else {
                goto strToTimeErrOut;
            }
            iStep = STEP_TIME, iDateTimeFlag |= FLAG_TIME;
            continue;
        }
            /* 强格式小时, 直接填充到小时信息中 */
        else if (iFlags == TOKEN_HOUR) {
            if ((iDateTimeFlag & FLAG_HOUR) == 0 && iVal < 24) {
                iHour = iVal, iDateTimeFlag |= FLAG_HOUR;
            } else {
                goto strToTimeErrOut;
            }
            iStep = STEP_TIME, iDateTimeFlag |= FLAG_TIME;
            continue;
        }
            /* 强格式分钟, 直接填充到分钟信息中 */
        else if (iFlags == TOKEN_MIN) {
            if ((iDateTimeFlag & FLAG_MIN) == 0 && iVal < 60) {
                iMinute = iVal, iDateTimeFlag |= FLAG_MIN;
            } else {
                goto strToTimeErrOut;
            }
            iStep = STEP_TIME, iDateTimeFlag |= FLAG_TIME;
            continue;
        }
            /* 强格式秒, 直接填充到秒信息中 */
        else if (iFlags == TOKEN_SEC) {
            if ((iDateTimeFlag & FLAG_SEC) == 0 && iVal < 60) {
                iSecond = iVal, iDateTimeFlag |= FLAG_SEC;
            } else {
                goto strToTimeErrOut;
            }
            iStep = STEP_TIME, iDateTimeFlag |= FLAG_TIME;
            continue;
        }

        /* 不是数值, 跳过 */
        if (iFlags != TOKEN_VAL) {
            continue;
        }

        /* 标记尾碰到时间分隔符, 则直接进入时间信息处理区 */
        if (IS_TIME_DELIM(pChar[iLen])) {
            if ((iDateTimeFlag & FLAG_TIME) == 0) {
                iStep = STEP_TIME, iDateTimeFlag |= FLAG_TIME;
            }
                /* 如果时间信息已获取, 且在处理日期状态, 则跳过 */
            else if (iStep == STEP_DATE) {
                iStep = STEP_OVER;
            }
        }
            /* 时间信息处理区, 但没有碰到时间分隔符, 则要切换处理区 */
        else if (iStep == STEP_TIME) {
            if (IS_TIME_DELIM(pChar[0])) {
                /* 最后一个字段, 状态保持 */;
            } else if ((iDateTimeFlag & FLAG_DATE) == 0) {
                iStep = STEP_DATE, iDateTimeFlag |= FLAG_DATE;
            }
                /* 如果日期信息已获取, 且在处理时间状态, 则跳过 */
            else {
                iStep = STEP_OVER;
            }
        }
            /* 默认情况下都是处理日期 */
        else {
            iStep = STEP_DATE;
        }

        /* 处理时间: 时分秒, 逐次检查(秒可能被省略) */
        if (iStep == STEP_TIME) {
            if ((iDateTimeFlag & FLAG_HOUR) == 0) {
                if (iVal >= 0 && iVal <= 23) {
                    iHour = iVal, iDateTimeFlag |= FLAG_HOUR;
                } else {
                    goto strToTimeErrOut;
                }
            } else if ((iDateTimeFlag & FLAG_MIN) == 0) {
                if (iVal >= 0 && iVal <= 59) {
                    iMinute = iVal, iDateTimeFlag |= FLAG_MIN;
                } else {
                    goto strToTimeErrOut;
                }
            } else if ((iDateTimeFlag & FLAG_SEC) == 0) {
                if (iVal >= 0 && iVal <= 59) {
                    iSecond = iVal, iDateTimeFlag |= FLAG_SEC;
                } else {
                    goto strToTimeErrOut;
                }
            } else /* 多余的时间数据, 则认为字符串不合法 */
            {
                goto strToTimeErrOut;
            }

            continue;

        } /* if (iStep == STEP_TIME) */

        /* 日期处理, 日期格式只有三种: 月日年, 日月年, 年月日, 03oct01?
         * 歧议日期一律按"月日年"顺序处理, 上例是01年10月3日而不是03年10月1日 */

        /* 四位数字年份, 毫无疑问直接填充 */
        if (iVal >= 1900 && iVal <= 2036) {
            if ((iDateTimeFlag & FLAG_YEAR) == 0) {
                iYear = iVal, iDateTimeFlag |= FLAG_YEAR;
            } else {
                goto strToTimeErrOut;
            }
        }
            /* 两位数字无争议年份, 填充 */
        else if (iVal == 0 || (iVal >= 70 && iVal <= 99)) {
            if ((iDateTimeFlag & FLAG_YEAR) == 0) {
                iYear = iVal, iDateTimeFlag |= FLAG_YEAR;
            } else {
                goto strToTimeErrOut;
            }
        } else if (iVal <= 12)    /* 小于12的数字, 年月日都可以 */
        {
            /* 写入月份读数时要检测日期读数冲突 */
            if ((iDateTimeFlag & FLAG_MONTH) == 0) {
                if ((iDateTimeFlag & FLAG_DAY) == 0 ||
                    iDay <= 29 + s_iMonthFix[iVal - 1]) {
                    iMonth = iVal, iDateTimeFlag |= FLAG_MONTH;
                } else if ((iDateTimeFlag & FLAG_YEAR) == 0) {
                    iYear = iVal, iDateTimeFlag |= FLAG_YEAR;
                } else {
                    goto strToTimeErrOut;
                }
            } else if ((iDateTimeFlag & FLAG_DAY) == 0) {
                iDay = iVal, iDateTimeFlag |= FLAG_DAY;
            } else if ((iDateTimeFlag & FLAG_YEAR) == 0) {
                iYear = iVal, iDateTimeFlag |= FLAG_YEAR;
            } else {
                goto strToTimeErrOut;
            }
        } else if (iVal <= 31 /* && iVal > 12 */) {
            /* 写入日期读数时要检测月份读数冲突 */
            if ((iDateTimeFlag & FLAG_DAY) == 0) {
                if ((iDateTimeFlag & FLAG_MONTH) == 0 ||
                    iVal <= 29 + s_iMonthFix[iMonth - 1]) {
                    iDay = iVal, iDateTimeFlag |= FLAG_DAY;
                } else if ((iDateTimeFlag & FLAG_YEAR) == 0) {
                    iYear = iVal, iDateTimeFlag |= FLAG_YEAR;
                } else {
                    goto strToTimeErrOut;
                }
            } else if ((iDateTimeFlag & FLAG_YEAR) == 0) {
                iYear = iVal, iDateTimeFlag |= FLAG_YEAR;
            } else {
                goto strToTimeErrOut;
            }
        } else if (iVal > 31 && iVal <= 36) {
            if ((iDateTimeFlag & FLAG_YEAR) == 0) {
                iYear = iVal, iDateTimeFlag |= FLAG_YEAR;
            } else {
                goto strToTimeErrOut;
            }
        } else /* 不合法的读数 */
        {
            goto strToTimeErrOut;
        }

    }

    /* 对于存在歧议的日期及时间, 目前的默认顺序是: 月份, 日期, 年份
     * 使用这个规则的原因在于, 月份的限制最严格(1~12), 其次日期(1~31), 年份任意,
     * 按这个顺序填入日期读数, 可以避免冲突数据的搬移操作,
     * 要改变这个默认规则, 只需在下面检查是否存在歧议, 然后修改顺序即可 */

    /* 年份读数转换为内部表示 */
    if (iDateTimeFlag & FLAG_YEAR) {
        if (iYear > 2036) {
            goto strToTimeErrOut;
        } else if (iYear >= 1900) {
            ptTime->tm.tm_year = iYear - 1900;
        } else if (iYear >= 0 && iYear < 36) {
            ptTime->tm.tm_year = iYear + 100;
        } else if (iYear >= 70 && iYear <= 99) {
            ptTime->tm.tm_year = iYear;
        } else {
            goto strToTimeErrOut;
        }
    }

    /* 月份读数也要转换为内部表示(0~11) */
    if (iDateTimeFlag & FLAG_MONTH) {
        if (iMonth < 1 || iMonth > 12) {
            goto strToTimeErrOut;
        }
        ptTime->tm.tm_mon = iMonth - 1;
    }

    /* 日期, 要查看是否满足条件 */
    if (iDateTimeFlag & FLAG_DAY) {
        if ((iDay > 29 + s_iMonthFix[ptTime->tm.tm_mon]) ||
            (ptTime->tm.tm_mon == 1 && !LEAP_YEAR(iYear) && iDay > 28)) {
            goto strToTimeErrOut;
        }
        ptTime->tm.tm_mday = iDay;
    } else {
        ptTime->tm.tm_mday = 1;
    }

    /* 时间值在赋值时都是合法的 */
    ptTime->tm.tm_hour = iHour;
    ptTime->tm.tm_min = iMinute;
    ptTime->tm.tm_sec = iSecond;

    ptTime->ts = mktime(&ptTime->tm);

    return iDateTimeFlag;

strToTimeErrOut:
    memset(ptTime, 0, sizeof(*ptTime));
    ptTime->tm.tm_mday = 1;
//    printf("Unrecognized date and/or time string:\n%s\n", timeStr);
    return 0;
}