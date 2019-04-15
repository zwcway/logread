%require "3.0.4"

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

void yyerror(char *s, ...);
int yylex();
%}

%debug

%locations

%union {
    int intval;
    double floatval;
    char *strval;
    int subtok;
}

/* token 关键字 */
%token TIDENTIFIER
%token TSTRING
%token TDATETIME
%token TFILE
%token TJSON
%token TURL
%token THOST
%token TIPV4
%token TIPV6
%token TINTEGER
%token TFLOAT
%token TDOUBLE
%token TLONG
%token TSHORT
%token TSIGNED
%token TCHAR
%token TUNSIGNED
%token TEND

%token FHOST
%token FLEVEL
%token FFILE
%token FTIME

%token TSPACE
%token TEQUAL
%token TJCLO
%token TQUT
%token TDQUT
%token TCOMMA
%token TLBRACKET
%token TRBRACKET
%token TLBRACE
%token TRBRACE

%token UNKNOWN

%token <strval> IDENTITY
%token <intval> INT_NUMBER
%token <floatval> FLOAT_NUMBER
%token <strval> STRING

%left ASSIGN

//%type <intval> decl_stmt bin_expr factor expr sentence id_list assign_stmt

%start statement

%%

/* 语法规则 */
statement: FLEVEL ' '         { printf("FLEVEL\n"); }
    ;


%%

void yyerror(char *s, ...) {
    fprintf(stderr, "error:%s\n", s);
}