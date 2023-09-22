#include <stdio.h>
#include "util.h"

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

