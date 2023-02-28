%{
#include <stdio.h>
#include "parser.tab.h"
%}

%option noyywrap

%%
[ \t]                   { continue; }
"."                     { return DOT; }
"("                     { return LPAREN; }
")"                     { return RPAREN; }
"\\"                    { return LAMBDA; }
[a-zA-Z][a-zA-Z0-9_]*   { return VAR; }
"\n"                    { return EOL; }

%%
