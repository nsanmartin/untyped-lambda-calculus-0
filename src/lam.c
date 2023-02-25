#include <stdlib.h>
#include <string.h>
#include <stdio.h>
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

Lstr ulam_get_form_name(const Lterm t[static 1]) {
    match(*t) {
        of(Lvar, _) return ulam_str("Variable");
        of(Labs, _, _) return ulam_str("Abstraction");
        of(Lapp, _, _) return ulam_str("Application");
    }
    LOG_INVALID_LTERM_AND_EXIT ;
}


const char* ulam_get_form_name_cstr(const Lterm t[static 1]) {
    return ulam_get_form_name(t).s;
}



/**
 * Factory methods
 **/

Lterm* ulam_new_var(Lstr x) {
    Lstr n = ulam_strdup(x);
    if (ulam_str_null(n)) { return 0x0; } 
    Lterm* rv = calloc(1, sizeof (Lterm));
    if (!rv) { ulam_str_free(n); return 0x0; }
    *rv = Lvar(n);
    return rv;
}

Lterm* ulam_new_abs(Lstr x, Lterm body[static 1]) {
    Lstr vn = ulam_strdup(x);
    if (ulam_str_null(vn)) { return 0x0; } 
    Lterm* b = ulam_clone(body);
    if (!b) { ulam_str_free(vn); return 0x0; }
    Lterm* rv = calloc(1, sizeof (Lterm));
    if (!rv) { ulam_str_free(vn); ulam_free_term(b); return 0x0; }
    *rv = Labs(vn, b);
    return rv;
}

Lterm* ulam_new_app(Lterm fun[static 1], Lterm param[static 1]) {
    Lterm* f = ulam_clone(fun);
    if (!f) { return 0x0; }
    Lterm* p = ulam_clone(param);
    if (!p) { ulam_free_term(f); return 0x0; }
    Lterm* rv = calloc(1, sizeof(Lterm));
    if (!rv) { ulam_free_term(f); ulam_free_term(p); return 0x0; }
    *rv = Lapp(f, p);
    return rv;
}


bool ulam_is_var_free_in(const Lterm t[static 1], Lstr n) {
    match(*t) {
        of(Lvar, name) return ulam_strcmp(n, *name) == 0;
        of(Labs, varname, body)
            return ulam_strcmp(n, *varname) != 0
                && ulam_is_var_free_in(*body, n);
        of(Lapp, f, param) 
            return ulam_is_var_free_in(*f, n)
                || ulam_is_var_free_in(*param, n);
    }
    LOG_INVALID_LTERM_AND_EXIT ;
}



const char var_reserved_char = '#';

int count_trailing_left(Lstr s, const char c) {
    const char* p = s.s;
    while (*p == c) { ++p; }
    return p - s.s;
}

int ulam_max_reserved_var_len(const Lterm t[static 1]) {
    match(*t) {
        of(Lvar, name) 
            return count_trailing_left(*name, var_reserved_char);
        of(Labs, _, body)
            return ulam_max_reserved_var_len(*body); //TODO: use S \ x if same?
        of(Lapp, fun, param)  {
            int fun_count = ulam_max_reserved_var_len(*fun); 
            int param_count = ulam_max_reserved_var_len(*param); 
            return fun_count > param_count ? fun_count : param_count;
        }
    }
    LOG_INVALID_LTERM_AND_EXIT ;
}


Lstr ulam_get_fresh_var_name(const Lterm t[static 1]) {
    const int len = ulam_max_reserved_var_len(t) + 1;
    char* rv = malloc(sizeof(char) * (len+ 1));
    if (!rv) { return ulam_str(0x0); }
    memset(rv, var_reserved_char, len);
    rv[len] = '\0';
    return ulam_allocated_str(rv);
}


/**
 * Rename
 */


int ulam_rename_var(Lterm t[static 1], Lstr varname, Lstr newname) {
    match(*t) {
        of(Lvar, name) {
            if (ulam_strcmp(varname, *name) == 0) {
                ulam_str_free(*name);
                *name = ulam_strdup(newname);
                if (ulam_str_null(*name)) { return -1; }

            }
            return 0;
        }

        of(Labs, tvarname, body) {
            if (ulam_strcmp(*tvarname, varname) == 0) {
                ulam_str_free(*tvarname);
                *tvarname = ulam_strdup(newname);
                if (ulam_str_null(*tvarname)) { return -1; }
            }
            return ulam_rename_var(*body, varname, newname);
        }
            
        of(Lapp, fun, param)  {
            return ulam_rename_var(*fun, varname, newname) 
                + ulam_rename_var(*param, varname, newname);
        }
    }
    LOG_INVALID_LTERM_AND_EXIT ;
}


//todo: free Lstr from Lterm (without freeing terms)
//
void ulam_free_term(Lterm* t) {
    match(*t) {
        of(Lvar, name) {
            ulam_str_free(*name);
            free(t);
            return;
        }

        of(Labs, varname, body) {
            ulam_str_free(*varname);
            ulam_free_term(*body);
            free(t);
            return;
        }
            
        of(Lapp, fun, param)  {
            ulam_free_term(*fun);
            ulam_free_term(*param);
            free(t);
            return;
        }
    }
    LOG_INVALID_LTERM_AND_EXIT ;
}

Lterm* ulam_clone(const Lterm t[static 1]) {
    match(*t) {
        of(Lvar, name) return ulam_new_var(*name);
        of(Labs, varname, body) return ulam_new_abs(*varname, *body);
        of(Lapp, fun, param)  return ulam_new_app(*fun, *param);
    }
    LOG_INVALID_LTERM_AND_EXIT ;
}


/**
 * Substitute
 */

Lterm*
ulam_substitute(const Lterm t[static 1], Lstr x, const Lterm s[static 1])
{
    match(*t) {
        of(Lvar, name) {
            if (ulam_strcmp(*name, x) == 0) {
                return ulam_clone(s);
            } else {
                return ulam_clone(t);
            }
        }

        of(Labs, varname, body) {
            if (ulam_strcmp(*varname, x) != 0
                && ulam_is_var_free_in(*body, x)) {

                const Lterm* r = s;
                if (ulam_is_var_free_in(s, x)) {
                    r = ulam_clone(r);
                    if (!r) { return 0x0; }
                    Lstr fresh_name = ulam_get_fresh_var_name(r);
                    if (ulam_str_null(fresh_name)) {
                        ulam_free_term((Lterm*)r); return 0x0;
                    }
                    ulam_rename_var((Lterm*)r, *varname, fresh_name);
                    ulam_str_free(fresh_name);
                }

                Lterm* subst = ulam_substitute(*body, x, r);
                if (s != r) { ulam_free_term((Lterm*)r); }
                if (!subst) { return 0x0; }
                Lstr vn = ulam_strdup(*varname);
                if (ulam_str_null(vn)) {
                    ulam_free_term(subst); return 0x0;
                }
                Lterm* rv = calloc(1, sizeof (Lterm));
                if (!rv) {
                    ulam_str_free(vn); ulam_free_term(subst); return 0x0;
                }
                *rv = Labs(vn, subst);
                return rv;
            } else { //x is captured by \x
                return ulam_clone(t);
            }
        }

        of(Lapp, f, p)  {
            Lterm* f_ = ulam_substitute(*f, x, s);
            if (!f_) { return 0x0; }
            Lterm* p_ = ulam_substitute(*p, x, s);
            if (!p_) { ulam_free_term(f_); return 0x0; }
            Lterm* rv = calloc(1, sizeof(Lterm));
            if (!rv) { ulam_free_term(f_); ulam_free_term(p_); return 0x0; }
            *rv = Lapp(f_, p_);
            return rv;
        }
    }
    LOG_INVALID_LTERM_AND_EXIT ;
}


bool ulam_are_identical(const Lterm t[static 1], const Lterm u[static 1]) {
    match(*t) {
        of(Lvar, tname) {
            match(*u) {
                of(Lvar, uname) {
                    return ulam_strcmp(*tname, *uname) == 0;
                }
                otherwise return false;
            }
        }
        of(Labs, tvarname, tbody) {
            match(*u) {
                of(Labs, uvarname, ubody) {
                    return ulam_strcmp(*tvarname, *uvarname) == 0 
                        && ulam_are_identical(*tbody, *ubody);
                }
                otherwise return false;
            }
        }
        of(Lapp, tf, tparam) {
            match(*u) {
                of(Lapp, uf, uparam) {
                    return ulam_are_identical(*tf, *uf)
                        && ulam_are_identical(*tparam, *uparam);
                }
                otherwise return false;
            }
        }
    }
    LOG_INVALID_LTERM_AND_EXIT ;
}

