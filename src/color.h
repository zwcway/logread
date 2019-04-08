#ifndef LOGR_COLOR_H
#define LOGR_COLOR_H

#define C_NONE                 "0"
#define C_BLACK                "0;30"
#define C_L_BLACK              "1;30"
#define C_RED                  "0;31"
#define C_L_RED                "1;31"
#define C_GREEN                "0;32"
#define C_L_GREEN              "1;32"
#define C_BROWN                "0;33"
#define C_YELLOW               "1;33"
#define C_BLUE                 "0;34"
#define C_L_BLUE               "1;34"
#define C_PURPLE               "0;35"
#define C_L_PURPLE             "1;35"
#define C_CYAN                 "0;36"
#define C_L_CYAN               "1;36"
#define C_GRAY                 "0;37"
#define C_WHITE                "1;37"
#define C_L_GRAY               "0;90"

#define C_BOLD                 "1"
#define C_UNDERLINE            "4"
#define C_BLINK                "5"
#define C_REVERSE              "7"
#define C_HIDE                 "8"
#define C_CLEAR                "2J"
#define C_CLRLINE              "\rK" //or "1K\r

#define PC_NONE             printf(C_NONE);     //关闭彩色字体
#define PC_RED              printf(C_RED);      //红色字体
#define PC_GREEN            printf(C_GREEN);    //绿色字体
#define PC_YELLOW           printf(C_YELLOW);   //黄色字体
#define PC_BLUE             printf(C_BLUE);     //蓝色字体

#endif