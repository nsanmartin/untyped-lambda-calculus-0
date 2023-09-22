%code requires {
    #include <stdio.h>
    #include <lam.h>

    extern FILE* yyin;

    extern int yylex(void);
    extern void yyerror(const char*);
}

%union {
    char* sval;
    Lterm* termval;
}


%token DOT
%token LPAREN
%token RPAREN
%token LAMBDA
%token <sval> VAR
%token EOL

%nterm <termval> term
%nterm <termval> expression

%%
expression:                                 { $$ = 0x0; }
    | expression term EOL                   {
        lam_print_term($2);
        puts("");
    }

    ;
term: VAR                                   {

       Lterm* var = lam_new_var(lam_str($1));
       //lam_print_term(var);
       $$ = var;
   }
   | LPAREN term term RPAREN                {
       Lterm* app = lam_new_app($2, $3);
       //lam_print_term(app);
       $$ = app;
   }
   | LPAREN LAMBDA VAR DOT term RPAREN      {
       Lterm* abs = lam_new_abs(lam_str($3), $5);
       //lam_print_term(abs);
       $$ = abs;
   }
   ;
%%

#ifndef LAM_LIB_PARSER
int main (void) {
    yyin = stdin;
    yyparse();
}
#endif
