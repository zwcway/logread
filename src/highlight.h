//
// Created by Administrator on 2019/3/13.
//

#ifndef LOGR_HIGHLIGHT_H
#define LOGR_HIGHLIGHT_H

#include <stdlib.h>
#include <ctype.h>
#include "utils.h"
#include "c-ctype.h"
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
#define HL_INTERNAL_HIGHLIGHT   "hl"


#define PRINTF_LENGTH(__buf, __str)       MAX_LINE - (__buf - *(__str))

#define SPRTF_STR_COLOR(__str, color, __val)        (*(__str)) += snprintf(*(__str), PRINTF_LENGTH(outputstr, __str), HL_START "%s" HL_END, color, __val)
#define SPRTF_STR(__str, __val)                     (*(__str)) += snprintf(*(__str), PRINTF_LENGTH(outputstr, __str), "%s", __val)

#define SPRTF_STR_CBUF(__str, __len, __col, __val)   if (__val) (*(__str)) += snprintf(*(__str), (size_t)__len, HL_START "%s" HL_END, __col, __val)
#define SPRTF_STR_BUF(__str, __len, __fmt, __val)   if (__val) (*(__str)) += snprintf(*(__str), (size_t)__len, __fmt, __val)


extern void sprtf_key_val (char **__str, const char *__key, const char *__val, bool print_space);

extern void sprtf_hl (const char *__buf, char **__str, int __len, const char *__key, Highlight *hl);

extern void parse_logr_colors (void);

extern void color_dict_free(void);

#endif //LOGR_HIGHLIGHT_H
