%option noyywrap yylineno stack nodefault
%x LOGSTART
%x BRACEVALUE
%x EQUALVALUE
%x BRACEVALUETYPE

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bison.h"

#define P(type) printf("get a " #type ": %s\n", yytext)
#define PB(flag)   printf("stack: [%d] " #flag "\n", enter_tracevalue)

int enter_tracevalue = 0, enter_equalvalue = 0;
char *sstr, *estr, *string;
%}

LONG        -?[0-9]+
DOUBLE      -?[0-9]*\.[0-9]+
LEVEL       (DEBUG|TRACE|NOTICE|WARNING|FATAL)
IPV4        ([0-9]{1,3}\.){3}[0-9]{1,3}
TIME_LONG   [0-2][0-9]:[0-5][0-9]:[0-5][0-9]
DATE_LONG   ([0-9]{4}|[0-9]{2})-[01][0-9]-[0-3][0-9]
DATE_TIME   {DATE_LONG}\ {TIME_LONG}
HOST        (([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\-]*[a-zA-Z0-9])\.)+([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\-]*[A-Za-z0-9])(:[0-9]+)?
URI         \/[A-Za-z0-9\-\._~:\/?#\[@!\$%&'\(\)\*\+,;=.]+
URL         http(s)?:\/\/({HOST})(\/{URI})?
FILE        \/[0-9a-zA-Z_/]+\.?[a-z0-9]+
FILELINE    {FILE}:[0-9]+
JSON        (\{.+\}|\[.+\])
MOBILE      (\+[0-9]{2})? ?1[3456789]12334455
TELPHONE    [^ \[\]\"'{}=\t\r\n :,]+
STRING      [^ \[\]\"'{}=\t\r\n :,]+
BACKTRACE   ({FILELINE}(\|(->|::|){STRING}))+(,{FILELINE}(\|(->|::|){STRING}))?

JSTRING     (\"([^"\\]|\\['"?\\]|\\u[0-9a-f]{4}|{UCN})*\"|\'([^'\\]|\\['"?\\]|\\u[0-9a-f]{4}|{UCN})*\')
JDOUBLE     {DOUBLE}
JLONG       {LONG}
JBOOL       "true"|"false"
JNULL       "null"

%%

" "                             { return TSPACE; }
[\t\n\r]                        ;

"["                             {
    // 括号压栈
    enter_tracevalue=1;
    // 开启处理括号的独立起始状态
    BEGIN(BRACEVALUE);
    P(LBRACKET1);return TLBRACKET;
}
<BRACEVALUE>"["                 {
    // 括号压栈
    enter_tracevalue++;
    // 标准状态下已经括号压栈了，因此此次的括号，当作字符串，交由下一个模式去处理
    yyless(0);
    P(LBRACKET2);return TLBRACKET;
}
<BRACEVALUE>"]"                 {
    // 括号出栈，并判断当前状态下的括号是否已经完全关闭
    if((--enter_tracevalue)<=0) {
        enter_tracevalue=0;
        // 已完全关闭，返回标准状态
        BEGIN(INITIAL);
        P(RBRACKET);return TRBRACKET;
    } else {
        // 未完全关闭，将下一个模式的字符串添加进来
        yymore();
    }
}
<BRACEVALUE>{DOUBLE}\]                        { if(enter_tracevalue==1) {yyless(yyleng-1);P(VAL_DOUBLE);return TDOUBLE;} }
<BRACEVALUE>{LONG}\]                          { if(enter_tracevalue==1) {yyless(yyleng-1);P(VAL_LONG);return TLONG;} }
<BRACEVALUE>{IPV4}\]                          { if(enter_tracevalue==1) {yyless(yyleng-1);P(VAL_IPV4);return TIPV4;} }
<BRACEVALUE>{DATE_TIME}\]                     { if(enter_tracevalue==1) {yyless(yyleng-1);P(VAL_DATETIME);return TDATETIME;} }
<BRACEVALUE>{FILELINE}\]                      { if(enter_tracevalue==1) {yyless(yyleng-1);P(VAL_FILELINE);return TFILE;} }
<BRACEVALUE>{HOST}\]                          { if(enter_tracevalue==1) {yyless(yyleng-1);P(VAL_HOST);return THOST;} }
<BRACEVALUE>{URI}\]                           { if(enter_tracevalue==1) {yyless(yyleng-1);P(VAL_URI);return TURL;} }
<BRACEVALUE>{URL}\]                           { if(enter_tracevalue==1) {yyless(yyleng-1);P(VAL_URL);return TURL;} }
<BRACEVALUE>{FILE}\]                          { if(enter_tracevalue==1) {yyless(yyleng-1);P(VAL_FILE);return TFILE;} }
<BRACEVALUE>[^\]]+\]            {
    {
        //计算括号栈，出现一个 [ 压栈，出现一个 ] 出栈。保证括号匹配，完全闭合。
        char *text = yytext + yyleng - 2;
        for(;yytext <= text;text--) {
            if (*text == '[') enter_tracevalue++;
            else if (*text == ']') enter_tracevalue--;
        }
    }
    if((--enter_tracevalue)>0) {
        // 括号未匹配，将下一个模式的字符串添加进来
        yymore();
    } else {
        // 括号已经匹配，回推字符 ]
        yyless(yyleng-1);P(VAL_STRING);return TSTRING;
    }
}
<BRACEVALUE>.              {
    yymore();
}
<BRACEVALUE>[\n\r]              {
    // 出现换行，强制结束不匹配的括号
    enter_tracevalue=0;
    // 跳出到标准状态
    BEGIN(INITIAL);
}

"["                             { return TLBRACKET; }
"]"                             { return TRBRACKET; }
"="                             { return TEQUAL; }
"{"                             { return TLBRACE; }
"}"                             { return TRBRACE; }
":"                             { return TJCLO; }
"'"                             { return TQUT; }
\"                              { return TDQUT; }
","                             { return TCOMMA; }
{LEVEL}                         { P(LEVEL);return FLEVEL; }
{DOUBLE}                        { P(DOUBLE);return TDOUBLE; }
{LONG}                          { P(LONG);return TLONG; }
{IPV4}                          { P(IPV4);return TIPV4; }
{DATE_TIME}                     { P(DATETIME);return TDATETIME; }
{FILELINE}                      { P(FILE);return TFILE; }
{FILE}                          { P(FILE);return TFILE; }
{HOST}                          { P(HOST);return THOST; }
{URL}                           { P(URL);return TURL; }
{URI}                           { P(URL);return TURL; }
{STRING}                        { P(STRING);return TSTRING; }
.                               { P(UNKNOWN); }
<<EOF>>                         { ECHO; yyterminate(); }


%%


#ifndef LOGR_LOGR_H
int main(int argc, char **argv)
{
    if (argc > 1) {
        if (!(yyin = fopen(argv[1], "r"))) {
            perror(argv[1]);
            return 1;
        }
    } else  {
        if (!(yyin = stdin)) {
            perror(argv[1]);
            return 1;
        }
    }
    while (yylex());
    return 0;
}
#endif