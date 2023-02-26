#ifndef __LAM_H_
#define __LAM_H_

#include <gc/gc.h>

typedef struct { const char* s; int alloc; } Lstr;

#define ulam_str(S) (Lstr){.s=S}
#define ulam_allocated_str(S) (Lstr){.s=S, .alloc=1}

static inline char* ulam_str_to_cstr(Lstr s) {
    return (char*)s.s;
}

static inline Lstr ulam_strdup(Lstr s) {
    const char* copy = s.s;
    int alloc = 0;
    if(s.alloc) {
        copy = GC_malloc(strlen(s.s) + 1);
        alloc = copy != 0x0;
    }
    return (Lstr) {.s=copy, .alloc=alloc};
}

static inline bool ulam_str_null(Lstr s) {
    return !s.s;
}

static inline int ulam_strcmp(Lstr s, Lstr t) {
    return strcmp(ulam_str_to_cstr(s), ulam_str_to_cstr(t));
}


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
