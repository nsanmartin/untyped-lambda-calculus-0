#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "lam.h"

#define LOG_INVALID_LTERM \
        fprintf( \
            stderr, \
            "\033[91m" \
            "Fatal error:\n============" \
            "\033[0m" \
            "\n\tInvalid term form.") \

#define LOG_INVALID_LTERM_AND_EXIT \
    LOG_INVALID_LTERM; exit(EXIT_FAILURE) \

long used_fresh_vars = 0;

Lstr ulam_get_form_name(const Lterm t[static 1]) {
    match(*t) {
        of(Lvar, _) return "Variable";
        of(Labs, _, _) return "Abstraction";
        of(Lapp, _, _) return "Application";
    }
    LOG_INVALID_LTERM_AND_EXIT ;
}


LatForm lam_term_form(Lat t) {
    LatForm rv = *(LatForm*)t;
    if (rv > LATAPP) {
        fprintf(
            stderr,
            "\033[91m"
            "Fatal error:\n============"
            "\033[0m"
            "\n\tInvalid term form: %d.\nExiting.\n",
            rv
        );
        exit(1);
    }
    return rv;
}

Lstr lam_term_form_name(Lat t) {
    switch (lam_term_form(t)) {
        case LATVAR:
            return "Variable";
        case LATABS:
            return "Abstraction";
        case LATAPP:
            return "Application";
        default:
            return "";
    }
}

/**
 * Factory methods
 **/

Lat lam_make_var(Lstr name) {
    LatVar* rv = malloc(sizeof(LatVar));
    if (!rv) { return NULL; }

    name = strdup(name);
    if (!name) {
        free (rv);
        return NULL;
    }
    *rv = (LatVar) { .form = LATVAR, .name = name };
    return (Lat)rv;
}


Lat lam_make_abs(Lstr var_name, Lat body) {
    LatAbs* rv = malloc(sizeof(LatAbs));
    if (!rv) { return NULL; }

    var_name = strdup(var_name);
    if (!var_name) {
        free (rv);
        return NULL;
    }
    *rv = (LatAbs) { .form = LATABS, .var_name = var_name, .body = body };
    return (Lat)rv;
}


Lat lam_make_app(Lat fun, Lat param) {
    LatApp* rv = malloc(sizeof(LatApp));
    if (!rv) { return NULL; }

    *rv = (LatApp) { .form = LATAPP, .fun= fun, .param = param };
    return (Lat)rv;
}

// Factory methods

/**
 * Dtor
 **/



void lam_free_var(LatVar* t) {
    free((char*)t->name);
    free(t);
}

void lam_free_abs(LatAbs* t) {
    free((char*)t->var_name);
    lam_free(t->body);
    free(t);
}

void lam_free_app(LatApp* t) {
    lam_free(t->fun);
    lam_free(t->param);
    free(t);
}


void lam_free(Lat t) {
    switch (lam_term_form(t)) {
        case LATVAR:
            lam_free_var((LatVar*)t);
            break;
        case LATABS:
            lam_free_abs((LatAbs*)t);
            break;
        case LATAPP:
            lam_free_app((LatApp*)t);
            break;
        default:
            fprintf(stderr, "(%s): Invalid lambda term form: %d\n", __func__, lam_term_form(t));
            break;
    }
}

// Dtor
 
/**
 * Getters
 */

Lstr lam_get_var_name(Lat t) { return ((LatVar*)t)->name; }
Lstr lam_get_abs_var_name(Lat t) { return ((LatAbs*)t)->var_name; }

// Getters
/**
 * x \in FV(t)
 */

bool ulam_is_var_free_in(const Lterm t[static 1], Lstr n) {
    match(*t) {
        of(Lvar, name) return strcmp(n, *name) == 0;
        of(Labs, varname, body)
            return strcmp(n, *varname) != 0
                && ulam_is_var_free_in(*body, n);
        of(Lapp, f, param) 
            return ulam_is_var_free_in(*f, n)
                || ulam_is_var_free_in(*param, n);
    }
    LOG_INVALID_LTERM_AND_EXIT ;
}

bool is_var_free_in_var(LatVar* t, Lstr var_name) {
    return strcmp(var_name, t->name) == 0;
}

bool is_var_free_in_abs(LatAbs* t, Lstr var_name) {
    return strcmp(var_name, t->var_name) != 0 && is_var_free_in(t->body, var_name);
}

bool is_var_free_in_app(LatApp* t, Lstr var_name) {
    return is_var_free_in(t->fun, var_name) || is_var_free_in(t->param, var_name);
}

bool is_var_free_in(Lat t, Lstr var_name) {
    switch (lam_term_form(t)) {
        case LATVAR:
            return is_var_free_in_var((LatVar*)t, var_name);
        case LATABS:
            return is_var_free_in_abs((LatAbs*)t, var_name);
        case LATAPP:
            return is_var_free_in_app((LatApp*)t, var_name);
        default:
            fprintf(stderr, "Invalid lambda term form\n");
            return false;
    }
}



const char var_reserved_char = '#';

int count_trailing_left(Lstr s, const char c) {
    const char* p = s;
    while (*p == c) { ++p; }
    return p - s;
}

int ulam_max_reserved_var_len(Lterm t[static 1]) {
    match(*t) {
        of(Lvar, name) 
            return count_trailing_left(*name, var_reserved_char);
        of(Labs, _, body)
            return max_reserved_var_len(*body); //TODO: use S \ x if same?
        of(Lapp, fun, param)  {
            int fun_count = max_reserved_var_len(*fun); 
            int param_count = max_reserved_var_len(*param); 
            return fun_count > param_count ? fun_count : param_count;
        }
    }
    LOG_INVALID_LTERM_AND_EXIT ;
}

int max_reserved_var_len_in_var(LatVar* t) {
    return count_trailing_left(t->name, var_reserved_char);
}

int max_reserved_var_len_in_abs(LatAbs* t) {
    return max_reserved_var_len(t->body);
    //TODO: use \x if same?
}
int max_reserved_var_len_in_app(LatApp* t) {
    int fun_count = max_reserved_var_len(t->fun); 
    int param_count = max_reserved_var_len(t->param); 
    return fun_count > param_count ? fun_count : param_count;
}
int max_reserved_var_len(Lat t) {
    switch (lam_term_form(t)) {
        case LATVAR:
            return max_reserved_var_len_in_var((LatVar*)t);
        case LATABS:
            return max_reserved_var_len_in_abs((LatAbs*)t);
        case LATAPP:
            return max_reserved_var_len_in_app((LatApp*)t);
        default:
            fprintf(stderr, "Invalid lambda term form\n");
            return false;
    }
}


Lstr ulam_get_fresh_var_name(Lterm t[static 1]) {
    const int len = ulam_max_reserved_var_len(t) + 1;
    char* rv = malloc(sizeof(char) * (len+ 1));
    if (!rv) { return NULL; }
    memset(rv, var_reserved_char, len);
    rv[len] = '\0';
    return rv;
}

Lstr get_fresh_var_name(Lat t) {
    const int len = max_reserved_var_len(t)  + 1;
    char* rv = malloc(sizeof(char) * (len+ 1));
    if (!rv) { return NULL; }
    memset(rv, var_reserved_char, len);
    rv[len] = '\0';
    return rv;
}

/**
 * Rename
 */


int ulam_rename_var(Lterm t[static 1], Lstr varname, Lstr newname) {
    match(*t) {
        of(Lvar, name) {
            if (strcmp(varname, *name) == 0) {
                //todo: check literal vs allocated strings
                //free((char*)*name);
                *name = strdup(newname);
                if (!*name) { return -1; }

            }
            return 0;
        }

        of(Labs, tvarname, body) {
            if (strcmp(*tvarname, varname) == 0) {
                // ^
                //free((char*)t->var_name);
                *tvarname = strdup(newname);
                if (!*tvarname) { return -1; }
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

void lam_rename_var_in_var(LatVar* t, Lstr var_name, Lstr new_name) {
    if (strcmp(var_name, t->name) == 0) {
        free((char*)t->name);
        t->name = strdup(new_name);
    }
}


void lam_rename_var_in_abs(LatAbs* t, Lstr var_name, Lstr new_name) {
    if (strcmp(t->var_name, var_name) == 0) {
        free((char*)t->var_name);
        t->var_name = strdup(new_name);
    }
    lam_rename_var(t->body, var_name, new_name);
}

void lam_rename_var_in_app(LatApp* t, Lstr var_name, Lstr new_name) {
    lam_rename_var(t->fun, var_name, new_name);
    lam_rename_var(t->param, var_name, new_name);
}

void lam_rename_var(Lat t, Lstr var_name, Lstr new_name) {
    switch (lam_term_form(t)) {
        case LATVAR:
            lam_rename_var_in_var((LatVar*)t, var_name, new_name);
        case LATABS:
            lam_rename_var_in_abs((LatAbs*)t, var_name, new_name);
        case LATAPP:
            lam_rename_var_in_app((LatApp*)t, var_name, new_name);
        default:
            fprintf(stderr, "Invalid lambda term form\n");
    }
}

// Rename

/**
 * Clone
 */

void ulam_free_term(Lterm* t) {
    match(*t) {
        of(Lvar, name) {
            free((char*)*name);
            free(t);
            return;
        }

        of(Labs, varname, body) {
            free((char*)*varname);
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
        of(Lvar, name) {
            Lstr n = strdup(*name);
            if (!n) { return 0x0; } 
            Lterm* rv = calloc(1, sizeof (Lterm));
            if (!rv) { free((char*)n); return 0x0; }
            *rv = Lvar(n);
            return rv;
        }

        of(Labs, varname, body) {
            Lstr vn = strdup(*varname);
            if (!vn) { return 0x0; } 
            Lterm* b = ulam_clone(*body);
            if (!b) { free((char*)vn); return 0x0; }
            Lterm* rv = calloc(1, sizeof (Lterm));
            if (!rv) { free((char*)vn); ulam_free_term(b); return 0x0; }
            *rv = Labs(vn, b);
            return rv;
        }
            
        of(Lapp, fun, param)  {
            Lterm* f = ulam_clone(*fun);
            if (!f) { return 0x0; }
            Lterm* p = ulam_clone(*param);
            if (!p) { ulam_free_term(f); return 0x0; }
            Lterm* rv = calloc(1, sizeof(Lterm));
            if (!rv) { ulam_free_term(f); ulam_free_term(p); return 0x0; }
            *rv = Lapp(f, p);
            return rv;
        }
    }
    LOG_INVALID_LTERM_AND_EXIT ;
}

Lat lam_clone_var(LatVar* t) { return lam_make_var(t->name); }
Lat lam_clone_abs(LatAbs* t) { return lam_make_abs(t->var_name, lam_clone(t->body)); }
Lat lam_clone_app(LatApp* t) {
    return lam_make_app(lam_clone(t->fun), lam_clone(t->param));
}


Lat lam_clone(Lat t) {
    switch (lam_term_form(t)) {
        case LATVAR:
            return lam_clone_var((LatVar*)t);
        case LATABS:
            return lam_clone_abs((LatAbs*)t);
        case LATAPP:
            return lam_clone_app((LatApp*)t);
        default:
            fprintf(stderr, "Invalid lambda term form\n");
            exit(EXIT_FAILURE);
    }
}

// Clone

/**
 * Substitute
 */

Lterm*
ulam_substitute(const Lterm t[static 1], Lstr x, const Lterm s[static 1])
{
    match(*t) {
        of(Lvar, name) {
            if (strcmp(*name, x) == 0) {
                return ulam_clone(s);
            } else {
                return ulam_clone(t);
            }
        }

        of(Labs, varname, body) {
            if (strcmp(*varname, x) != 0
                    && ulam_is_var_free_in(*body, x)) {
                if (ulam_is_var_free_in(s, x)) {
                    Lat s2 = ulam_clone(s);
                    if (!s2) { return 0x0; }
                    Lstr fresh_name = get_fresh_var_name(s2);
                    if (!fresh_name) { ulam_free_term(s2); return 0x0; }
                    //ulam_rename_var(s2, var_name, fresh_name);
                    free((char*)fresh_name);
                    //Lat rv = lam_make_abs(var_name, lam_substitute(abs->body, var_name, s2));
                    //lam_free(s2);
                    //return rv;
                    return 0x0;

                } else {
                    //return lam_make_abs(
                    //var_name, lam_substitute(abs->body, var_name, s));
                    return 0x0;
                }
            }
        }

        of(Lapp, _, _)  {
            return 0x0;
        }
    }
    LOG_INVALID_LTERM_AND_EXIT ;
}

Lat lam_substitute_in_var(LatVar* t, Lstr var_name, Lat s) {
    if (strcmp(t->name, var_name) == 0) {
        return lam_clone(s);
    } else {
        return lam_clone(t);
    }
}

Lat lam_substitute_in_abs(LatAbs* abs, Lstr var_name, Lat s) {
    //TODO: check for NULL?
    if (strcmp(abs->var_name, var_name) != 0 && is_var_free_in(abs, var_name)) {
        if (is_var_free_in(s, var_name)) {
            Lat s2 = lam_clone(s);
            Lstr fresh_name = get_fresh_var_name(s2);
            lam_rename_var(s2, var_name, fresh_name);
            free((char*)fresh_name);
            Lat rv = lam_make_abs(var_name, lam_substitute(abs->body, var_name, s2));
            lam_free(s2);
            return rv;

        } else {
            //lam_substitute(&abs->body, var_name, s);
            return lam_make_abs(var_name, lam_substitute(abs->body, var_name, s));
        }
    }
    // else, same var, do nothing
    return lam_clone(abs);
}

Lat lam_substitute_in_app(LatApp* app, Lstr var_name, Lat s) {
    return lam_make_app(lam_substitute(app->fun, var_name, s), lam_substitute(app->param, var_name, s));
}

Lat lam_substitute(Lat t, Lstr var_name, Lat s) {
    switch (lam_term_form(t)) {
        case LATVAR:
            return lam_substitute_in_var((LatVar*)t, var_name, s);
        case LATABS:
            return lam_substitute_in_abs((LatAbs*)t, var_name, s);
        case LATAPP:
            return lam_substitute_in_app((LatApp*)t, var_name, s);
        default:
            fprintf(stderr, "(%s) Invalid lambda term form\n", __func__);
    }
    return NULL;
}

// Substitute

/**
 * lam_are_identical
 */

bool ulam_are_identical(const Lterm t[static 1], const Lterm u[static 1]) {
    match(*t) {
        of(Lvar, tname) {
            match(*u) {
                of(Lvar, uname) {
                    return strcmp(*tname, *uname) == 0;
                }
                otherwise return false;
            }
        }
        of(Labs, tvarname, tbody) {
            match(*u) {
                of(Labs, uvarname, ubody) {
                    return strcmp(*tvarname, *uvarname) == 0 
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

bool lam_are_identical_vars(LatVar* t, LatVar* u) {
    return strcmp(t->name, u->name) == 0;
}

bool lam_are_identical_abss(LatAbs* t, LatAbs* u) {
    if (strcmp(t->var_name, u->var_name) != 0) {
        return false;
    }

    return lam_are_identical(t->body, u->body);
}

bool lam_are_identical_apps(LatApp* t, LatApp* u) {
    return lam_are_identical(t->fun, u->fun) && lam_are_identical(t->param, u->param);
}

bool lam_are_identical(Lat t, Lat u) {
    LatForm tform = lam_term_form(t);
    LatForm uform = lam_term_form(u);
    if (tform != uform) { return false; }

    switch (tform) {
        case LATVAR:
            return lam_are_identical_vars((LatVar*)t, (LatVar*)u);
        case LATABS:
            return lam_are_identical_abss((LatAbs*)t, (LatAbs*)u);
        case LATAPP:
            return lam_are_identical_apps((LatApp*)t, (LatApp*)u);

        default:
            fprintf(stderr, "Invalid lambda term form\n");
    }
    return false;
}

// are_identical
