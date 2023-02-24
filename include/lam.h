#ifndef __LAM_H_
#define __LAM_H_

#include "datatype99.h"

/* Lat stands for lamda term */

typedef struct { const char* s; int alloc; } Lstr;
//typedef const char* Lstr;

#define ulam_str(S) (Lstr){.s=S}
#define ulam_allocated_str(S) (Lstr){.s=S, .alloc=1}

// #define ulam_str_to_cstr(LS) (char*)LS.s
static inline char* ulam_str_to_cstr(Lstr s) {
    return (char*)s.s;
}

//#define ulam_strdup(S)  ulam_allocated_str(strdup(ulam_str_to_cstr(S)))
static inline Lstr ulam_strdup(Lstr s) {
    if(s.alloc) return ulam_allocated_str(strdup(ulam_str_to_cstr(s)));
    return ulam_str(s.s);

}

static inline bool ulam_str_null(Lstr s) {
    return !s.s;
}
static inline void ulam_str_free(Lstr s) { if (s.alloc) free((char*)s.s); }

static inline int ulam_strcmp(Lstr s, Lstr t) {
    return strcmp(ulam_str_to_cstr(s), ulam_str_to_cstr(t));
}

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
const char* ulam_get_form_name_cstr(const Lterm t[static 1]) ;
bool ulam_is_var_free_in(const Lterm t[static 1], Lstr n) ;
int ulam_max_reserved_var_len(const Lterm t[static 1]) ;
Lstr ulam_get_fresh_var_name(const Lterm t[static 1]) ;
Lterm* ulam_clone(const Lterm t[static 1]) ;
bool ulam_are_identical(const Lterm t[static 1], const Lterm u[static 1]);
int ulam_rename_var(Lterm t[static 1], Lstr varname, Lstr newname) ;


Lterm* ulam_new_app(Lterm fun[static 1], Lterm param[static 1]) ;
Lterm* ulam_new_abs(Lstr x, Lterm body[static 1]) ;
Lterm* ulam_new_var(Lstr x) ;

Lterm*
ulam_substitute(const Lterm t[static 1], Lstr x, const Lterm s[static 1]);

void ulam_free_term(Lterm* t) ;

#endif // __LAM_H_
