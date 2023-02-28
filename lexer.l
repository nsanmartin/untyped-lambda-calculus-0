%{
#include <stdio.h>
%}

%option noyywrap

%%
[ \t]   ;
"."     { printf("dot "); }
"("     {  printf("left paren "); }
")"     {  printf("right paren "); }
"\\"     {  printf("LAMBDA "); }
[a-zA-Z][a-zA-Z0-9_]* { printf("VAR "); }

%%

int main () {
    yylex();
}
