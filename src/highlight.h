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


#define PRINTF_LENGTH(__buf)       (MAX_LINE - (__buf)->offset - 1)
#define PRINTF_NEXT(__buf)         (((__buf)->outputstr) + (__buf)->offset)

#define SPRTF_STR_COLOR(__buf, color, __val)        ((__buf)->offset) += snprintf(PRINTF_NEXT(__buf), PRINTF_LENGTH(__buf), HL_START "%s" HL_END, color, __val)
#define SPRTF_STR(__buf, __val)                     ((__buf)->offset) += snprintf(PRINTF_NEXT(__buf), PRINTF_LENGTH(__buf), "%s", __val)

#define SPRTF_STR_CBUF(__buf, __col, __val)   if (__val) ((__buf)->offset) += snprintf(PRINTF_NEXT(__buf), PRINTF_LENGTH(__buf), HL_START "%s" HL_END, __col, __val)
#define SPRTF_STR_BUF(__buf, __fmt, __val)   if (__val) ((__buf)->offset) += snprintf(PRINTF_NEXT(__buf), PRINTF_LENGTH(__buf), __fmt, __val)


extern void sprtf_key_val (OutputBuffer *__str, const char *__key, const char *__val, bool print_space);

extern void sprtf_hl (OutputBuffer *__str, const char *__key, const Highlight *hl);

extern void parse_logr_colors (void);

extern void color_dict_free(void);

#endif //LOGR_HIGHLIGHT_H
