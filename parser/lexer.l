%{
#include <stdio.h>
#include "parser.tab.h"
%}

%option noyywrap
%option nounput
%option noinput

%%
[ \t]                   { continue; }
"."                     { return DOT; }
"("                     { return LPAREN; }
")"                     { return RPAREN; }
"\\"                    { return LAMBDA; }
[a-zA-Z][a-zA-Z0-9_]*   {
    char* text = lam_strdup_str(strlen(yytext), yytext);
    yylval.sval = text;
    return VAR;
    }
"\n"                    { return EOL; }

%%

void set_input_string(const char* in) {
  yy_scan_string(in);
}

void end_lexical_scan(void) {
  yy_delete_buffer(YY_CURRENT_BUFFER);
}

