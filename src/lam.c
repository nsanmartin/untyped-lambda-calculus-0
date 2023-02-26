#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "lam.h"

#define LOG_INVALID_LTERM                                              \
        fprintf(                                                       \
            stderr,                                                    \
            "\033[91m"                                                 \
            "ulam fatal error:\n================"                      \
            "\033[0m"                                                  \
            "\n\tInvalid term form.\n"                                 \
                "file: %s"                                             \
                ":%d\n"                                                \
                "func: %s\n",                                          \
                __FILE__,                                              \
                __LINE__,                                              \
                __func__)

#define LOG_INVALID_LTERM_AND_EXIT                                     \
    LOG_INVALID_LTERM; exit(EXIT_FAILURE)

long used_fresh_vars = 0;

Lstr ulam_get_form_name(const LatTerm t[static 1]) {
    match(*t) {
        of(LatVar, _) return ulam_str("Variable");
        of(LatAbs, _, _) return ulam_str("Abstraction");
        of(LatApp, _, _) return ulam_str("Application");
    }
    LOG_INVALID_LTERM_AND_EXIT ;
}

Lstr lam_get_form_name(const Lterm t[static 1]) {
    switch(t->tag) {
        case Lvartag: return ulam_str("Variable");
        case Labstag: return ulam_str("Abstraction");
        case Lapptag: return ulam_str("Application");
        default: LOG_INVALID_LTERM_AND_EXIT ;
    }
}


const char* ulam_get_form_name_cstr(const LatTerm t[static 1]) {
    return ulam_get_form_name(t).s;
}

const char* lam_get_form_name_cstr(const Lterm t[static 1]) {
    return lam_get_form_name(t).s;
}



/**
 * Factory methods
 **/

LatTerm* ulam_new_var(Lstr x) {
    Lstr n = ulam_strdup(x);
    if (ulam_str_null(n)) { return 0x0; } 
    LatTerm* rv = calloc(1, sizeof (LatTerm));
    if (!rv) { ulam_str_free(n); return 0x0; }
    *rv = LatVar(n);
    return rv;
}

//todo: use this in news methods??
//int lam_init_var(Lterm t[static 1], Lstr x) {
//    Lstr n = ulam_strdup(x);
//    if (ulam_str_null(n)) { return -1; } 
//    *t = (Lterm) {
//        .tag=Lvartag,
//        .var=(Lvar) { .name=n }
//    };
//    return 0;
//}
//
//int lam_init_abs(Lterm t[static 1], Lstr x, Lterm body[static 1]) {
//    Lstr vn = ulam_strdup(x);
//    if (ulam_str_null(vn)) { return -1; } 
//    Lterm* b = lam_clone(body);
//    if (!b) { ulam_str_free(vn); return -1; }
//    *t = (Lterm) { .tag = Labstag, .abs= (Labs) {.vname=vn, .body=b}};
//    return 0;
//}
//
//int
//lam_init_app(Lterm t[static 1], Lterm fun[static 1], Lterm param[static 1])
//{
//    Lterm* f = lam_clone(fun);
//    if (!f) { return -1; }
//    Lterm* p = lam_clone(param);
//    if (!p) { lam_free_term(f); return -1; }
//    *t = (Lterm) {
//        .tag=Lapptag,
//        .app=(Lapp) {.fun=f, .param=p}
//    };
//    return 0;
//}

LatTerm* ulam_new_abs(Lstr x, LatTerm body[static 1]) {
    Lstr vn = ulam_strdup(x);
    if (ulam_str_null(vn)) { return 0x0; } 
    LatTerm* b = ulam_clone(body);
    if (!b) { ulam_str_free(vn); return 0x0; }
    LatTerm* rv = calloc(1, sizeof (LatTerm));
    if (!rv) { ulam_str_free(vn); ulam_free_term(b); return 0x0; }
    *rv = LatAbs(vn, b);
    return rv;
}

LatTerm* ulam_new_app(LatTerm fun[static 1], LatTerm param[static 1]) {
    LatTerm* f = ulam_clone(fun);
    if (!f) { return 0x0; }
    LatTerm* p = ulam_clone(param);
    if (!p) { ulam_free_term(f); return 0x0; }
    LatTerm* rv = calloc(1, sizeof(LatTerm));
    if (!rv) { ulam_free_term(f); ulam_free_term(p); return 0x0; }
    *rv = LatApp(f, p);
    return rv;
}


Lterm* lam_new_var(Lstr x) {
    Lstr n = ulam_strdup(x);
    if (ulam_str_null(n)) { return 0x0; } 
    Lterm* rv = calloc(1, sizeof (Lterm));
    if (!rv) { ulam_str_free(n); return 0x0; }
    *rv = (Lterm) { .tag=Lvartag, . var = (Lvar) { .name = n }};
    return rv;
}

Lterm* lam_new_abs(Lstr x, Lterm body[static 1]) {
    Lstr vn = ulam_strdup(x);
    if (ulam_str_null(vn)) { return 0x0; } 
    Lterm* b = lam_clone(body);
    if (!b) { ulam_str_free(vn); return 0x0; }
    Lterm* rv = calloc(1, sizeof (Lterm));
    if (!rv) { ulam_str_free(vn); lam_free_term(b); return 0x0; }
    *rv = (Lterm) { .tag = Labstag, .abs= (Labs) {.vname=vn, .body=b}};
    return rv;
}

Lterm* lam_new_app(Lterm fun[static 1], Lterm param[static 1]) {
    Lterm* f = lam_clone(fun);
    if (!f) { return 0x0; }
    Lterm* p = lam_clone(param);
    if (!p) { lam_free_term(f); return 0x0; }
    Lterm* rv = calloc(1, sizeof(*rv));
    if (!rv) { lam_free_term(f); lam_free_term(p); return 0x0; }
    *rv = (Lterm) {.tag=Lapptag, .app=(Lapp){.fun=f, .param=p}};
    return rv;
}


bool ulam_is_var_free_in(const LatTerm t[static 1], Lstr n) {
    match(*t) {
        of(LatVar, name) return ulam_strcmp(n, *name) == 0;
        of(LatAbs, varname, body)
            return ulam_strcmp(n, *varname) != 0
                && ulam_is_var_free_in(*body, n);
        of(LatApp, f, param) 
            return ulam_is_var_free_in(*f, n)
                || ulam_is_var_free_in(*param, n);
    }
    LOG_INVALID_LTERM_AND_EXIT ;
}

bool lam_is_var_free_in(const Lterm t[static 1], Lstr n) {

    switch(t->tag) {
        case Lvartag: {return ulam_strcmp(n, t->var.name) == 0;}
        case Labstag: {
            return ulam_strcmp(n, t->abs.vname) != 0
                && lam_is_var_free_in(t->abs.body, n);
        }
        case Lapptag: {
            return lam_is_var_free_in(t->app.fun, n)
                || lam_is_var_free_in(t->app.param, n);
        }
        default: LOG_INVALID_LTERM_AND_EXIT ;
    }

}


const char var_reserved_char = '#';

int count_trailing_left(Lstr s, const char c) {
    const char* p = s.s;
    while (*p == c) { ++p; }
    return p - s.s;
}

int ulam_max_reserved_var_len(const LatTerm t[static 1]) {
    match(*t) {
        of(LatVar, name) 
            return count_trailing_left(*name, var_reserved_char);
        of(LatAbs, _, body)
            return ulam_max_reserved_var_len(*body); //TODO: use S \ x if same?
        of(LatApp, fun, param)  {
            int fun_count = ulam_max_reserved_var_len(*fun); 
            int param_count = ulam_max_reserved_var_len(*param); 
            return fun_count > param_count ? fun_count : param_count;
        }
    }
    LOG_INVALID_LTERM_AND_EXIT ;
}

int lam_max_reserved_var_len(const Lterm t[static 1]) {
    switch(t->tag) {
        case Lvartag: {
            return count_trailing_left(t->var.name, var_reserved_char);
        }
        case Labstag: {
            //TODO: use S \ x if same?
            return lam_max_reserved_var_len(t->abs.body);
        }
        case Lapptag: {
            int fun_count = lam_max_reserved_var_len(t->app.fun); 
            int param_count = lam_max_reserved_var_len(t->app.param); 
            return fun_count > param_count ? fun_count : param_count;
        }
        default: LOG_INVALID_LTERM_AND_EXIT ;
    }
}


Lstr ulam_get_fresh_var_name(const LatTerm t[static 1]) {
    const int len = ulam_max_reserved_var_len(t) + 1;
    char* rv = malloc(sizeof(char) * (len+ 1));
    if (!rv) { return ulam_str(0x0); }
    memset(rv, var_reserved_char, len);
    rv[len] = '\0';
    return ulam_allocated_str(rv);
}


Lstr lam_get_fresh_var_name(const Lterm t[static 1]) {
    const int len = lam_max_reserved_var_len(t) + 1;
    char* rv = malloc(sizeof(char) * (len+ 1));
    if (!rv) { return ulam_str(0x0); }
    memset(rv, var_reserved_char, len);
    rv[len] = '\0';
    return ulam_allocated_str(rv);
}

/**
 * Rename
 */


int ulam_rename_var(LatTerm t[static 1], Lstr varname, Lstr newname) {
    match(*t) {
        of(LatVar, name) {
            if (ulam_strcmp(varname, *name) == 0) {
                ulam_str_free(*name);
                //*name = ulam_strdup(newname);
                name->s = ulam_strdup_str(newname);
                if (ulam_str_null(*name)) { return -1; }

            }
            return 0;
        }

        of(LatAbs, tvarname, body) {
            if (ulam_strcmp(*tvarname, varname) == 0) {
                ulam_str_free(*tvarname);
                *tvarname = ulam_strdup(newname);
                if (ulam_str_null(*tvarname)) { return -1; }
            }
            return ulam_rename_var(*body, varname, newname);
        }
            
        of(LatApp, fun, param)  {
            return ulam_rename_var(*fun, varname, newname) 
                + ulam_rename_var(*param, varname, newname);
        }
    }
    LOG_INVALID_LTERM_AND_EXIT ;
}

int lam_rename_var(Lterm t[static 1], Lstr varname, Lstr newname) {
    switch(t->tag) {
        case Lvartag: {
            if (ulam_strcmp(varname, t->var.name) == 0) {
                ulam_str_free(t->var.name);
                //*name = ulam_strdup(newname);
                t->var.name.s = ulam_strdup_str(newname);
                if (ulam_str_null(t->var.name)) { return -1; }

            }
            return 0;
        }
        case Labstag: {
            if (ulam_strcmp(t->abs.vname, varname) == 0) {
                ulam_str_free(t->abs.vname);
                t->abs.vname.s = ulam_strdup_str(newname);
                if (ulam_str_null(t->abs.vname)) { return -1; }
            }
            return lam_rename_var(t->abs.body, varname, newname);
        }
        case Lapptag: {
            return lam_rename_var(t->app.fun, varname, newname) 
                + lam_rename_var(t->app.param, varname, newname);
                      }
        default: LOG_INVALID_LTERM_AND_EXIT ;
    }
}


//todo: free Lstr from LatTerm (without freeing terms)
//
void ulam_free_term(LatTerm* t) {
    match(*t) {
        of(LatVar, name) {
            ulam_str_free(*name);
            free(t);
            return;
        }

        of(LatAbs, varname, body) {
            ulam_str_free(*varname);
            ulam_free_term(*body);
            free(t);
            return;
        }
            
        of(LatApp, fun, param)  {
            ulam_free_term(*fun);
            ulam_free_term(*param);
            free(t);
            return;
        }
    }
    LOG_INVALID_LTERM_AND_EXIT ;
}

void lam_free_term(Lterm* t) {
    switch(t->tag) {
        case Lvartag: {
            ulam_str_free(t->var.name);
            free(t);
            break;
        }
        case Labstag: {
            ulam_str_free(t->abs.vname);
            lam_free_term(t->abs.body);
            free(t);
            break;
        }
        case Lapptag: {
            lam_free_term(t->app.fun);
            lam_free_term(t->app.param);
            free(t);
            break;
        }
        default: LOG_INVALID_LTERM_AND_EXIT ;
    }
}

LatTerm* ulam_clone(const LatTerm t[static 1]) {
    match(*t) {
        of(LatVar, name) return ulam_new_var(*name);
        of(LatAbs, varname, body) return ulam_new_abs(*varname, *body);
        of(LatApp, fun, param)  return ulam_new_app(*fun, *param);
    }
    LOG_INVALID_LTERM_AND_EXIT ;
}


Lterm* lam_clone(const Lterm t[static 1]) {
    //todo
    switch(t->tag) {
        case Lvartag: {return lam_new_var(t->var.name);}
        case Labstag: {return lam_new_abs(t->abs.vname, t->abs.body);}
        case Lapptag: {return lam_new_app(t->app.fun, t->app.param);}
        default: LOG_INVALID_LTERM_AND_EXIT ;
    }
}

/**
 * Substitute
 */

LatTerm*
ulam_substitute(const LatTerm t[static 1], Lstr x, const LatTerm s[static 1])
{
    match(*t) {
        of(LatVar, name) {
            if (ulam_strcmp(*name, x) == 0) {
                return ulam_clone(s);
            } else {
                return ulam_clone(t);
            }
        }

        of(LatAbs, varname, body) {
            if (ulam_strcmp(*varname, x) != 0
                && ulam_is_var_free_in(*body, x)) {

                const LatTerm* r = s;
                if (ulam_is_var_free_in(s, x)) {
                    r = ulam_clone(r);
                    if (!r) { return 0x0; }
                    Lstr fresh_name = ulam_get_fresh_var_name(r);
                    if (ulam_str_null(fresh_name)) {
                        ulam_free_term((LatTerm*)r); return 0x0;
                    }
                    ulam_rename_var((LatTerm*)r, *varname, fresh_name);
                    ulam_str_free(fresh_name);
                }

                LatTerm* subst = ulam_substitute(*body, x, r);
                if (s != r) { ulam_free_term((LatTerm*)r); }
                if (!subst) { return 0x0; }
                Lstr vn = ulam_strdup(*varname);
                if (ulam_str_null(vn)) {
                    ulam_free_term(subst); return 0x0;
                }
                LatTerm* rv = calloc(1, sizeof (LatTerm));
                if (!rv) {
                    ulam_str_free(vn); ulam_free_term(subst); return 0x0;
                }
                *rv = LatAbs(vn, subst);
                return rv;
            } else { //x is captured by \x
                return ulam_clone(t);
            }
        }

        of(LatApp, f, p)  {
            LatTerm* f_ = ulam_substitute(*f, x, s);
            if (!f_) { return 0x0; }
            LatTerm* p_ = ulam_substitute(*p, x, s);
            if (!p_) { ulam_free_term(f_); return 0x0; }
            LatTerm* rv = calloc(1, sizeof(LatTerm));
            if (!rv) { ulam_free_term(f_); ulam_free_term(p_); return 0x0; }
            *rv = LatApp(f_, p_);
            return rv;
        }
    }
    LOG_INVALID_LTERM_AND_EXIT ;
}

Lterm*
lam_substitute(const Lterm t[static 1], Lstr x, const Lterm s[static 1]) {
    switch(t->tag) {
        case Lvartag: {
            if (ulam_strcmp(t->var.name, x) == 0) {
                return lam_clone(s);
            } else {
                return lam_clone(t);
            }
        }
        case Labstag: {
            if (ulam_strcmp(t->abs.vname, x) != 0
                && lam_is_var_free_in(t->abs.body, x)) {

                const Lterm* r = s;
                if (lam_is_var_free_in(s, x)) {
                    r = lam_clone(r);
                    if (!r) { return 0x0; }
                    Lstr fresh_name = lam_get_fresh_var_name(r);
                    if (ulam_str_null(fresh_name)) {
                        lam_free_term((Lterm*)r); return 0x0;
                    }
                    lam_rename_var((Lterm*)r, t->abs.vname, fresh_name);
                    ulam_str_free(fresh_name);
                }

                Lterm* subst = lam_substitute(t->abs.body, x, r);
                if (s != r) { lam_free_term((Lterm*)r); }
                if (!subst) { return 0x0; }
                Lstr vn = ulam_strdup(t->abs.vname);
                if (ulam_str_null(vn)) {
                    lam_free_term(subst); return 0x0;
                }
                Lterm* rv = calloc(1, sizeof (*rv));
                if (!rv) {
                    ulam_str_free(vn); lam_free_term(subst); return 0x0;
                }
                *rv = (Lterm) {
                    .tag=Labstag,
                    .abs=(Labs) {.vname=vn, .body=subst}
                };
                return rv;
            } else { //x is captured by \x
                return lam_clone(t);
            }
        }
        case Lapptag: {
            Lterm* f_ = lam_substitute(t->app.fun, x, s);
            if (!f_) { return 0x0; }
            Lterm* p_ = lam_substitute(t->app.param, x, s);
            if (!p_) { lam_free_term(f_); return 0x0; }
            Lterm* rv = calloc(1, sizeof(*rv));
            if (!rv) { lam_free_term(f_); lam_free_term(p_); return 0x0; }
            *rv = (Lterm) {
                .tag=Lapptag,
                .app = (Lapp){ .fun=f_, .param=p_ }
            };
            return rv;
        }
        default: LOG_INVALID_LTERM_AND_EXIT ;
    }
}


bool ulam_are_identical(const LatTerm t[static 1], const LatTerm u[static 1]) {
    match(*t) {
        of(LatVar, tname) {
            match(*u) {
                of(LatVar, uname) {
                    return ulam_strcmp(*tname, *uname) == 0;
                }
                otherwise return false;
            }
        }
        of(LatAbs, tvarname, tbody) {
            match(*u) {
                of(LatAbs, uvarname, ubody) {
                    return ulam_strcmp(*tvarname, *uvarname) == 0 
                        && ulam_are_identical(*tbody, *ubody);
                }
                otherwise return false;
            }
        }
        of(LatApp, tf, tparam) {
            match(*u) {
                of(LatApp, uf, uparam) {
                    return ulam_are_identical(*tf, *uf)
                        && ulam_are_identical(*tparam, *uparam);
                }
                otherwise return false;
            }
        }
    }
    LOG_INVALID_LTERM_AND_EXIT ;
}

bool lam_are_identical(const Lterm t[static 1], const Lterm u[static 1]) {
    if (t->tag != u->tag) return false;

    switch(t->tag) {
        case Lvartag: {
            return ulam_strcmp(t->var.name, u->var.name) == 0;
        }
        case Labstag: {
            return ulam_strcmp(t->abs.vname, u->abs.vname) == 0 
                && lam_are_identical(t->abs.body, u->abs.body);
        }
        case Lapptag: {
            return lam_are_identical(t->app.fun, u->app.fun)
                && lam_are_identical(t->app.param, u->app.param);
        }
        default: LOG_INVALID_LTERM_AND_EXIT ;
    }
}

void ulam_print_term(const LatTerm t[static 1]) {
    match(*t) {
        of(LatVar, name) {
            printf("%s", ulam_str_to_cstr(*name));
            return;
        }
        of(LatAbs, varname, body) {
            printf("(\\%s.", ulam_str_to_cstr(*varname));
            ulam_print_term(*body);
            printf(")");
            return;
        }
        of(LatApp, fun, param) {
            printf("(");
            ulam_print_term(*fun);
            printf(" ");
            ulam_print_term(*param);
            printf(")");
            return;
        }
    }
    LOG_INVALID_LTERM_AND_EXIT ;
}

void lam_print_term(const Lterm t[static 1]) {
    switch(t->tag) {
        case Lvartag: {
            printf("%s", ulam_str_to_cstr(t->var.name));
            break;
        }
        case Labstag: {
            printf("(\\%s.", ulam_str_to_cstr(t->abs.vname));
            lam_print_term(t->abs.body);
            printf(")");
            break;
        }
        case Lapptag: {
            printf("(");
            lam_print_term(t->app.fun);
            printf(" ");
            lam_print_term(t->app.param);
            printf(")");
            break;
        }
        default: LOG_INVALID_LTERM_AND_EXIT ;
    }
}


char* ulam_term_to_string(const LatTerm t[static 1]) {
    match(*t) {
        of(LatVar, name) {
            char* rv;
            if (asprintf(&rv, "%s", ulam_str_to_cstr(*name)) == -1) {
                return 0x0;
            }
            return rv;
        }
        of(LatAbs, varname, body) {
            char* bstr = ulam_term_to_string(*body);
            if (!bstr) {
                return 0x0;
            }

            char* rv;
            if(asprintf(&rv, "(\\%s.%s)", ulam_str_to_cstr(*varname), bstr) == -1) {
                return 0x0;
            }
            free(bstr);
            return rv;
        }
        of(LatApp, fun, param) {
            char* fstr = ulam_term_to_string(*fun);
            if (!fstr) { return 0x0; }
            char* pstr = ulam_term_to_string(*param);
            if (!pstr) { free(fstr); return 0x0; }

            char* rv;
            if(asprintf(&rv, "(%s %s)", fstr, pstr) == -1) {
                return 0x0;
            }
            free(fstr);
            free(pstr);
            return rv;
        }
    }
    LOG_INVALID_LTERM_AND_EXIT ;
}

char* lam_term_to_string(const Lterm t[static 1]) {
    switch(t->tag) {
        case Lvartag: {
            char* rv;
            if (asprintf(&rv, "%s", ulam_str_to_cstr(t->var.name)) == -1) {
                return 0x0;
            }
            return rv;
        }
        case Labstag: {
            char* bstr = lam_term_to_string(t->abs.body);
            if (!bstr) { return 0x0; }
            char* rv;
            if(asprintf(
                &rv,
                "(\\%s.%s)",
                ulam_str_to_cstr(t->abs.vname),
                bstr
                ) == -1
            ) {
                return 0x0;
            }
            free(bstr);
            return rv;
        }
        case Lapptag: {
            char* fstr = lam_term_to_string(t->app.fun);
            if (!fstr) { return 0x0; }
            char* pstr = lam_term_to_string(t->app.param);
            if (!pstr) { free(fstr); return 0x0; }

            char* rv;
            if(asprintf(&rv, "(%s %s)", fstr, pstr) == -1) {
                return 0x0;
            }
            free(fstr);
            free(pstr);
            return rv;
        }
        default: LOG_INVALID_LTERM_AND_EXIT ;
    }
}
