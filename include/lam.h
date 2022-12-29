#ifndef __LAM_H_
#define __LAM_H_

/* Lat stands for lamda term */

typedef const char* Lstr;

typedef void* Lat;
typedef enum { LATVAR = 0, LATABS = 1, LATAPP = 2 }       LatForm;
typedef struct { LatForm form; Lstr name; }               LatVar;
typedef struct { LatForm form; Lstr var_name; Lat body; } LatAbs;
typedef struct { LatForm form; Lat fun, param; }          LatApp;

LatForm lam_term_form(Lat t);
Lstr lam_term_form_name(Lat t);
Lat lam_make_var(Lstr name);
Lat lam_make_abs(Lstr var_name, Lat body);
Lat lam_make_app(Lat fun, Lat param);

bool is_var_free_in(Lat t, Lstr var_name);
void lam_free(Lat t);

int max_reserved_var_len(Lat t) ;
Lstr get_fresh_var_name(Lat t) ;
#endif // __LAM_H_
