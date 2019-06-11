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

#define HL_INTERNAL_KEY         "k"
#define HL_INTERNAL_VAL         "v"
#define HL_INTERNAL_BACKGROUND  "b"
#define HL_INTERNAL_OPERATOR    "e"
#define HL_INTERNAL_HIGHLIGHT   "hl"

#define HL_COLORS_DEFAULT       "k=" C_CYAN \
                                ":v=" C_DARKGRAY \
                                ":*l,=" C_LIGHTWHITE \
                                ":*l,WARNING=" C_LIGHTYELLOW \
                                ":*l,FATAL=" C_LIGHTRED \
                                ":*time=" C_CYAN \
                                ":*time.ps_invoke=" C_LIGHTBLUE \
                                ":*errno,=" C_LIGHTRED \
                                ":*errno,0=" C_DARKGRAY

#define PRINTF_LENGTH(__buf)       (MAX_LINE - (__buf)->offset - 1)
#define PRINTF_NEXT(__buf)         (((__buf)->outputstr) + (__buf)->offset)

#define SPRTF_STR_CBUF(__buf, __col, __val)         if (__val) { \
((__buf)->offset) += snprintf(PRINTF_NEXT(__buf), PRINTF_LENGTH(__buf), HL_START "%s" HL_END, __col, __val); \
}

#define SPRTF_STR_BUF(__buf, __fmt, __val)          if (__val) { \
((__buf)->offset) += snprintf(PRINTF_NEXT(__buf), PRINTF_LENGTH(__buf), __fmt, __val); \
}

#define SPRTF_STR_COLOR(__buf, __col, __val)        SPRTF_STR_CBUF(__buf, __col, __val)
#define SPRTF_STR(__buf, __val)                     SPRTF_STR_BUF(__buf, "%s", __val)
#define SPRTF_STR_OR_COLOR(__buf, __col, __val)            do { \
    if (__col) { \
        SPRTF_STR_COLOR(__buf, __col, __val); \
    } else { \
        SPRTF_STR(__buf, __val); \
    } \
} while(0)

extern bool sprtf_key_val (OutputBuffer *__str, const char *__key, const char *__val, int opt);

extern void sprtf_hl (OutputBuffer *__str, const char *__key, const Highlight *hl);

extern void parse_logr_colors (void);

extern void color_dict_free(void);

#endif //LOGR_HIGHLIGHT_H
