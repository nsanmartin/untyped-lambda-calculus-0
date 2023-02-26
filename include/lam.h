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


static inline const char* ulam_strdup_str(Lstr s) {
    if(s.alloc) return strdup(ulam_str_to_cstr(s));
    return s.s;

}

static inline bool ulam_str_null(Lstr s) {
    return !s.s;
}
static inline void ulam_str_free(Lstr s) { if (s.alloc) free((char*)s.s); }

static inline int ulam_strcmp(Lstr s, Lstr t) {
    return strcmp(ulam_str_to_cstr(s), ulam_str_to_cstr(t));
}


datatype(
    LatTerm,
    (LatVar, Lstr),
    (LatAbs, Lstr, LatTerm*),
    (LatApp, LatTerm*, LatTerm*)
);

Lstr ulam_get_form_name(const LatTerm t[static 1]) ;
const char* ulam_get_form_name_cstr(const LatTerm t[static 1]) ;
bool ulam_is_var_free_in(const LatTerm t[static 1], Lstr n) ;
int ulam_max_reserved_var_len(const LatTerm t[static 1]) ;
Lstr ulam_get_fresh_var_name(const LatTerm t[static 1]) ;
LatTerm* ulam_clone(const LatTerm t[static 1]) ;
bool ulam_are_identical(const LatTerm t[static 1], const LatTerm u[static 1]);
int ulam_rename_var(LatTerm t[static 1], Lstr varname, Lstr newname) ;


LatTerm* ulam_new_app(LatTerm fun[static 1], LatTerm param[static 1]) ;
LatTerm* ulam_new_abs(Lstr x, LatTerm body[static 1]) ;
LatTerm* ulam_new_var(Lstr x) ;

LatTerm*
ulam_substitute(const LatTerm t[static 1], Lstr x, const LatTerm s[static 1]);

void ulam_free_term(LatTerm* t) ;

void ulam_print_term(const LatTerm t[static 1]) ;
char* ulam_term_to_string(const LatTerm t[static 1]) ;

typedef enum { Lvartag, Labstag, Lapptag } Lamtag;

typedef struct Lterm Lterm;

typedef struct { Lstr name; } Lvar;
typedef struct { Lstr vname; Lterm* body; } Labs;
typedef struct { Lterm* fun; Lterm* param; } Lapp;

typedef struct Lterm {
    Lamtag tag;
    union {
        Lvar var;
        Labs abs;
        Lapp app;
    }; //term;
} Lterm;

#define LVAR(NAME)                                                      \
    (Lterm){ .tag=Lvartag, .var=(Lvar){.name=NAME}} 

#define LABS(VNAME, BODY)                                               \
    (Lterm){.tag=Labstag, .abs=(Labs){.vname=VNAME, .body=&BODY}}

#define LAPP(FUN, PARAM)                                                \
    (Lterm) {.tag=Lapptag, .app=(Lapp){.fun=&FUN,.param=&PARAM}}


Lstr lam_get_form_name(const Lterm t[static 1]) ;
void lam_free_term(Lterm* t) ;
Lterm* lam_clone(const Lterm t[static 1]) ;
Lterm* lam_new_var(Lstr x) ;
Lterm* lam_new_abs(Lstr x, Lterm body[static 1]) ;
Lterm* lam_new_app(Lterm fun[static 1], Lterm param[static 1]) ;

int lam_init_var(Lterm t[static 1], Lstr x) ;
int lam_init_abs(Lterm t[static 1], Lstr x, Lterm body[static 1]) ;
int
lam_init_app(Lterm t[static 1], Lterm fun[static 1], Lterm param[static 1]);
char* lam_term_to_string(const Lterm t[static 1]) ;
bool lam_is_var_free_in(const Lterm t[static 1], Lstr n) ;
int lam_max_reserved_var_len(const Lterm t[static 1]) ;
Lstr lam_get_fresh_var_name(const Lterm t[static 1]) ;
int lam_rename_var(Lterm t[static 1], Lstr varname, Lstr newname) ;
const char* lam_get_form_name_cstr(const Lterm t[static 1]) ;
bool lam_are_identical(const Lterm t[static 1], const Lterm u[static 1]) ;
Lterm*
lam_substitute(const Lterm t[static 1], Lstr x, const Lterm s[static 1]) ;
#endif // __LAM_H_
