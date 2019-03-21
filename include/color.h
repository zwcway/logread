#ifndef LOGR_COLOR_H
#define LOGR_COLOR_H

#define C_NONE                 "\e[0m"
#define C_BLACK                "\e[0;30m"
#define C_L_BLACK              "\e[1;30m"
#define C_RED                  "\e[0;31m"
#define C_L_RED                "\e[1;31m"
#define C_GREEN                "\e[0;32m"
#define C_L_GREEN              "\e[1;32m"
#define C_BROWN                "\e[0;33m"
#define C_YELLOW               "\e[1;33m"
#define C_BLUE                 "\e[0;34m"
#define C_L_BLUE               "\e[1;34m"
#define C_PURPLE               "\e[0;35m"
#define C_L_PURPLE             "\e[1;35m"
#define C_CYAN                 "\e[0;36m"
#define C_L_CYAN               "\e[1;36m"
#define C_GRAY                 "\e[0;37m"
#define C_WHITE                "\e[1;37m"
 
#define C_BOLD                 "\e[1m"
#define C_UNDERLINE            "\e[4m"
#define C_BLINK                "\e[5m"
#define C_REVERSE              "\e[7m"
#define C_HIDE                 "\e[8m"
#define C_CLEAR                "\e[2J"
#define C_CLRLINE              "\r\e[K" //or "\e[1K\r


#define PC_NONE             printf(C_NONE);     //关闭彩色字体
#define PC_RED              printf(C_RED);      //红色字体
#define PC_GREEN            printf(C_GREEN);    //绿色字体
#define PC_YELLOW           printf(C_YELLOW);   //黄色字体
#define PC_BLUE             printf(C_BLUE);     //蓝色字体



#endif