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

Lstr lam_get_form_name(const Lterm t[static 1]) {
    switch(t->tag) {
        case Lvartag: return ulam_str("Variable");
        case Labstag: return ulam_str("Abstraction");
        case Lapptag: return ulam_str("Application");
        default: LOG_INVALID_LTERM_AND_EXIT ;
    }
}



const char* lam_get_form_name_cstr(const Lterm t[static 1]) {
    return lam_get_form_name(t).s;
}



/**
 * Factory methods
 **/



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


//todo: free Lstr from Lterm (without freeing terms)
//

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
