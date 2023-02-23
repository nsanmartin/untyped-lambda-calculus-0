#ifndef __LAM_H_
#define __LAM_H_

#include "datatype99.h"

/* Lat stands for lamda term */

typedef const char* Lstr;

typedef void* Lat;
typedef enum { LATVAR = 0, LATABS = 1, LATAPP = 2 }       LatForm;
typedef struct { LatForm form; Lstr name; }               LatVar;
typedef struct { LatForm form; Lstr var_name; Lat body; } LatAbs;
typedef struct { LatForm form; Lat fun, param; }          LatApp;


datatype(
    Lterm,
    (Lvar, Lstr),
    (Labs, Lstr, Lterm*),
    (Lapp, Lterm*, Lterm*)
);

Lstr ulam_get_form_name(const Lterm t[static 1]) ;
bool ulam_is_var_free_in(const Lterm t[static 1], Lstr n) ;
int ulam_max_reserved_var_len(Lterm t[static 1]) ;
Lstr ulam_get_fresh_var_name(Lterm t[static 1]) ;
Lterm* ulam_clone(const Lterm t[static 1]) ;
bool ulam_are_identical(const Lterm t[static 1], const Lterm u[static 1]);
int ulam_rename_var(Lterm t[static 1], Lstr varname, Lstr newname) ;

LatForm lam_term_form(Lat t);
Lstr lam_term_form_name(Lat t);
Lat lam_make_var(Lstr name);
Lat lam_make_abs(Lstr var_name, Lat body);
Lat lam_make_app(Lat fun, Lat param);

bool is_var_free_in(Lat t, Lstr var_name);
void lam_free(Lat t);

int max_reserved_var_len(Lat t) ;
Lstr get_fresh_var_name(Lat t) ;
void lam_rename_var(Lat t, Lstr var_name, Lstr new_name) ;
Lstr lam_get_var_name(Lat t) ;
Lstr lam_get_abs_var_name(Lat t) ;

Lat lam_substitute(Lat t, Lstr var_name, Lat s);

Lat lam_clone(Lat t);
bool lam_are_identical(Lat t, Lat u) ;
#endif // __LAM_H_
