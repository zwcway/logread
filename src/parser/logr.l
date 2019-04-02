%option noyywrap yylineno
%x LOGSTART
%x BRACEVALUE
%x EQUALVALUE
%x JSONVALUE

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "bison.c"
//#include "type.h"

#define TO_OPEN  1
#define TO_CLOSE 2
#define TO_EQUAL  3
#define TO_SPER  4
#define TO_STRING  5
#define TO_KEY  5
#define TO_DOUBLE  6
#define TO_LONG  7
#define TO_LBRACE  8
#define TO_RBRACE 9
#define TO_JCLO 10
#define TO_DATETIME 11
#define TO_FILE 12
#define TO_HOST 13
#define TO_URL 13
#define TO_IP 14
#define TO_JSON 14

#define P(type) printf("get a " #type ": %s\n", yytext)

//int old_status;
//void yyerror(char *s, ...);

int enter_tracevalue = 0, enter_equalvalue = 0;
%}

LEVEL       (DEBUG|TRACE|NOTICE|WARNING|FATAL):
IPV4        ([0-9]{1,3}\.){3}[0-9]{1,3}
TIME_LONG   [0-2][0-9]:[0-5][0-9]:[0-5][0-9]
DATE_LONG   ([0-9]{4}|[0-9]{2})-[01][0-9]-[0-3][0-9]
DATE_TIME   {DATE_LONG}\ {TIME_LONG}
HOST        (([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\-]*[a-zA-Z0-9])\.)+([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\-]*[A-Za-z0-9])(:[0-9]+)?
URL         (http(s)?:\/\/)?[A-Za-z0-9.-]+(\.[A-Za-z0-9\.-]+)+[A-Za-z0-9\-\._~:/?#[@!\$&'\(\)\*\+,;=.]+
URI         \/[A-Za-z0-9\-\._~:/?#[@!\$&'\(\)\*\+,;=.]+
FILE        \/[0-9a-zA-Z_/]+\.?[a-z0-9]+
FILELINE    {FILE}:[0-9]+
JSON        (\{.+\}|\[.+\])

%%


" "                     ;
[\t\n\r]                ;

\[                      BEGIN(BRACEVALUE);enter_tracevalue++;
<BRACEVALUE>"\]"                if((enter_tracevalue--)<=1) { BEGIN(INITIAL); }
<BRACEVALUE>[0-9]+\.[0-9]+      P(TO_DOUBLE);return TO_DOUBLE;
<BRACEVALUE>[0-9]+              P(TO_LONG);return TO_LONG;
<BRACEVALUE>{IPV4}              P(TO_IP);return TO_IP;
<BRACEVALUE>{DATE_TIME}         P(TO_DATETIME);return TO_DATETIME;
<BRACEVALUE>{FILE}              P(TO_FILE);return TO_FILE;
<BRACEVALUE>{FILELINE}          P(TO_FILE);return TO_FILE;
<BRACEVALUE>{URL}               P(TO_URL);return TO_URL;
<BRACEVALUE>{URI}               P(TO_URL);return TO_URL;
<BRACEVALUE>{HOST}              P(TO_HOST);return TO_HOST;
<BRACEVALUE>.                   P(UNKNOWN);


{IPV4}                P(TO_IP);return TO_IP;
{DATE_TIME}           P(TO_DATETIME);return TO_DATETIME;
[a-zA-Z_][a-zA-Z0-9_]*  P(TO_KEY);return TO_KEY;
=                     P(TO_EQUAL);return TO_EQUAL;
\[                    P(TO_OPEN);return TO_OPEN;
\]                    P(TO_CLOSE);return TO_CLOSE;
{                     P(TO_LBRACE);return TO_LBRACE;
}                     P(TO_RBRACE);return TO_RBRACE;
:                     P(TO_JCLO);return TO_JCLO;
.                       P(UNKNOWN);


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