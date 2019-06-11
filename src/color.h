#ifndef LOGR_COLOR_H
#define LOGR_COLOR_H

#define C_START                "\33["
#define C_END                  "\33[K"

#define C_NONE                 "0"
#define C_BLACK                "30"
#define C_RED                  "31"
#define C_GREEN                "32"
#define C_YELLOW               "33"
#define C_BLUE                 "34"
#define C_PURPLE               "35"
#define C_CYAN                 "36"
#define C_LIGHTGRAY            "37"
#define C_DEFAULT              "39"
#define C_DARKGRAY             "90"
#define C_LIGHTRED             "91"
#define C_LIGHTGREEN           "92"
#define C_LIGHTYELLOW          "93"
#define C_LIGHTBLUE            "94"
#define C_LIGHTMAGENTA         "95"
#define C_LIGHTCYAN            "96"
#define C_LIGHTWHITE           "97"

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


#define COLOR(__str, __col)           C_START __col "m" __str C_END C_START C_NONE "m" C_END

#endif