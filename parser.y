%code requires {
    #include <stdio.h>

    extern FILE* yyin;

    extern int yylex(void);
    extern void yyerror(const char*);
}

%token DOT
%token LPAREN
%token RPAREN
%token LAMBDA
%token VAR
%token EOL

%%
expression:
    | expression term EOL                   { printf("EXPRESSION\n"); }
    ;
term: VAR                                   { printf("VAR "); }
   | LPAREN term term RPAREN                { printf("APP "); }
   | LPAREN LAMBDA VAR DOT term RPAREN      { printf("ABS "); }
   ;
%%

int main () {
    yyin = stdin;
    yyparse();
}

void yyerror(const char* s) {
    fprintf(stderr, "error: %s\n", s);
}
