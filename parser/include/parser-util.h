#ifndef __PARSER_UTIL_H__
#define __PARSER_UTIL_H__

#include "lam.h"

void set_input_string(const char* in) ;
void end_lexical_scan(void) ;

void yyerror(const char* s) ;
int yyparse(void) ;


/* Declarations */
void set_input_string(const char* in);
void end_lexical_scan(void);

/* This function parses a string */
Lstr parse_string(const char* in) ;

void set_lam_term(Lterm* t) ;
Lterm* get_lam_term(void) ;
#endif
