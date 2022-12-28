#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lam.h"

LatForm lam_term_form(Lat t) { return *(LatForm*)t; }

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

void lam_free(Lat t) {
    fprintf(stderr, "%s no implemented\n", __func__);
}
// Dtor
 
//bool is_var_free_in(Lat t, Lstr var_name) {
//}
//
//bool is_var_free_in_var(LatVar t, Lstr var_name)
/**
 * subst
 */
int lam_substitute(Lat* t, Lstr var_name, Lat s) {
    return 0;
}

int lam_substitute_in_var(LatVar** t, Lstr var_name, Lat s) {
    if (strcmp((*t)->name, var_name) == 0) {
        lam_free(*t);
        *t = s;
    }
    return 0;
}

int lam_substitute_in_abs(LatAbs** t, Lstr var_name, Lat s) {
    if (strcmp((*t)->var_name, var_name) != 0) {

    }
    

}
// subst
