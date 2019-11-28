//
// Created by Administrator on 2019/11/28.
//

#ifndef LOGR_TIME_H
#define LOGR_TIME_H

#include <time.h>
#include <string.h>
#include <ctype.h>


#define TOKEN_INIT      0
#define TOKEN_VAL       1
#define TOKEN_SEC       2
#define TOKEN_MIN       3
#define TOKEN_HOUR      4
#define TOKEN_HALFDAY   5
#define TOKEN_DAY       6
#define TOKEN_MONTH     7
#define TOKEN_YEAR      8
#define TOKEN_WEEKDAY   9

#define STEP_INIT       0
#define STEP_DATE       1
#define STEP_TIME       2
#define STEP_OVER       3

#define FLAG_SEC        0x0001
#define FLAG_MIN        0x0002
#define FLAG_HOUR       0x0004
#define FLAG_HALFDAY    0x0008
#define FLAG_DAY        0x0010
#define FLAG_MONTH      0x0020
#define FLAG_YEAR       0x0040
#define FLAG_TIME       0x0100
#define FLAG_DATE       0x0200
#define FLAG_ALPHA      0x1000

typedef struct Time {
    long ts;
    struct tm tm;
} Time;

static char s_cWeekDay[] = "MonTueWedThuFriSatSun";
static char s_cWeekDayU[] = "MONTUEWEDTHUFRISATSUN";
static char s_cWeekDayL[] = "montuewedthufrisatsun";
static char s_cMonth[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
static char s_cMonthU[] = "JANFEBMARAPRMAYJUNJULAUTSEPOCTNOVDEC";
static char s_cMonthL[] = "janfebmaraprmayjunjulaugsepoctnovdec";
static int s_iMonthFix[] = {2, 0, 2, 1, 2, 1, 2, 2, 1, 2, 1, 2};

/**
 * 是否是时间分隔符
 */
#define IS_TIME_DELIM(a)  ((a) == ':')

/**
 * 是否是日期分隔符
 */
#define IS_DATE_DELIM(a)  (isspace(a) || (a) == ',' || (a) == '.' || (a) == '-' || (a) == '/' || (a) == '\\')

/**
 * 是否是闰年
 */
#define LEAP_YEAR(year)   (((year) % 400 == 0) || (((year) % 100 != 0) && ((year) % 4 == 0)))


extern int strtotime(const char *timeStr, Time *ptTime);

#endif //LOGR_TIME_H
