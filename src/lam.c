#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "lam.h"

long used_fresh_vars = 0;

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


int max_reserved_var_len_in_var(LatVar* t) { return count_trailing_left(t->name, var_reserved_char); }
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
            return lam_rename_var_in_var((LatVar*)t, var_name, new_name);
        case LATABS:
            return lam_rename_var_in_abs((LatAbs*)t, var_name, new_name);
        case LATAPP:
            return lam_rename_var_in_app((LatApp*)t, var_name, new_name);
        default:
            fprintf(stderr, "Invalid lambda term form\n");
    }
}

// Rename

/**
 * Clone
 */

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
    }
}

// Clone

/**
 * Substitute
 */

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

/*
 * to_string
 */

Lstr lam_to_string_in_var(LatVar* t) {
    return strdup(t->name);
}


Lstr lam_to_string_in_abs(LatAbs* t) {
    Lstr body = lam_to_string(t->body);
    int vlen = strlen(t->var_name);
    int blen = strlen(body);
    int len = vlen + blen + 5;
    char* rv = malloc(sizeof(char) * len);
    snprintf(rv, len, "(\\%s.%s)", t->var_name, body);
    free((char*)body);
    return rv;

}


Lstr lam_to_string_in_app(LatApp* t) {
    Lstr fun = lam_to_string(t->fun);
    Lstr param = lam_to_string(t->param);
    int len = strlen(fun) + strlen(param) + 3;
    char* rv = malloc(sizeof(char) * len);
    snprintf(rv, len, "(%s%s)", fun, param);
    return rv;
}

Lstr lam_to_string(Lat t) {
    switch (lam_term_form(t)) {
        case LATVAR:
            return lam_to_string_in_var((LatVar*)t);
        case LATABS:
            return lam_to_string_in_abs((LatAbs*)t);
        case LATAPP:
            return lam_to_string_in_app((LatApp*)t);
        default:
            fprintf(stderr, "(%s) Invalid lambda term form\n", __func__);
    }
    return NULL;
}

// to_string
