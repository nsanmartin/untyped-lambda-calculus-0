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

%%
expression:
    | expression term EOL                   { printf(" EXPRESSION\n"); }
    ;
term: VAR                                   {

       printf("VAR: ");
       Lterm* var = lam_new_var(lam_str($1));
       lam_print_term(var);
       $$ = var;
       puts("");
   }
   | LPAREN term term RPAREN                {
       printf("APP: ");
       Lterm* app = lam_new_app($2, $3);
       lam_print_term(app);
       $$ = app;
       puts("");
   }
   | LPAREN LAMBDA VAR DOT term RPAREN      {
       printf("ABS: ");
       Lterm* abs = lam_new_abs(lam_str($3), $5);
       lam_print_term(abs);
       $$ = abs;
       puts("");
   }
   ;
%%

int main () {
    yyin = stdin;
    yyparse();
}

void yyerror(const char* s) {
    fprintf(stderr, "error: %s\n", s);
}


/* Declarations */
void set_input_string(const char* in);
void end_lexical_scan(void);

/* This function parses a string */
int parse_string(const char* in) {
  set_input_string(in);
  int rv = yyparse();
  end_lexical_scan();
  return rv;
}

