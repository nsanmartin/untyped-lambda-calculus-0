#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "utest.h"

#include "lam.h"

#define X ulam_str("x")
#define Y ulam_str("y")
#define Z ulam_str("z")
#define T ulam_str("t")
#define U ulam_str("u")
#define V ulam_str("v")
#define FRESH_VAR ulam_str("fresh var")
#define RES_CHAR1 ulam_str("#")
#define RES_CHAR2 ulam_str("##")
#define RES_CHAR3 ulam_str("###")
#define RES_CHAR4 ulam_str("####")
#define RES_CHAR5 ulam_str("#####")

UTEST_MAIN()

UTEST(term_form_name,A) {
    Lterm x = Lvar(X);
    ASSERT_STREQ(ulam_str_to_cstr(ulam_get_form_name(&x)), "Variable");

    Lterm lx_x = Labs(X, &x);
    ASSERT_STREQ(ulam_str_to_cstr(ulam_get_form_name(&lx_x)), "Abstraction");

    Lterm applx_x__x = Lapp(&lx_x, &x);
    ASSERT_STREQ(ulam_str_to_cstr(ulam_get_form_name(&applx_x__x)), "Application");
}



UTEST(lam_free_vars, Abs) {

    Lterm x = Lvar(X);
    Lterm lx_x = Labs(X, &x);
    ASSERT_FALSE(ulam_is_var_free_in(&lx_x, X));
    ASSERT_FALSE(ulam_is_var_free_in(&lx_x, FRESH_VAR));


    Lterm ly_x = Labs(Y, &x);
    ASSERT_TRUE (ulam_is_var_free_in(&ly_x, X));

    Lterm lz_ly_x = Labs(Z, &ly_x);
    ASSERT_TRUE(ulam_is_var_free_in( &lz_ly_x, X));
    ASSERT_FALSE(ulam_is_var_free_in(&lz_ly_x, Y));
    ASSERT_FALSE(ulam_is_var_free_in(&lz_ly_x, Z));

    Lterm lz_lx_x = Labs(Z, &lx_x);
    ASSERT_FALSE(ulam_is_var_free_in(&lz_lx_x, X));
    ASSERT_FALSE(ulam_is_var_free_in(&lz_lx_x, Y));
    ASSERT_FALSE(ulam_is_var_free_in(&lz_lx_x, Z));

    Lterm lx_ly_x = Labs(X, &ly_x);
    ASSERT_FALSE(ulam_is_var_free_in(&lx_ly_x, X));
    ASSERT_FALSE(ulam_is_var_free_in(&lx_ly_x, Y));
    ASSERT_FALSE(ulam_is_var_free_in(&lx_ly_x, Z));
}


UTEST(LamTermsFixture, free_vars_app) {
    Lterm x = Lvar(X);
    Lterm xx = Lapp(&x, &x);
    ASSERT_FALSE(ulam_is_var_free_in(&xx, Y));
    ASSERT_TRUE(ulam_is_var_free_in(&xx, X));

    Lterm y = Lvar(Y);
    Lterm xy = Lapp(&x, &y);

    ASSERT_FALSE(ulam_is_var_free_in(&xy, FRESH_VAR));
    ASSERT_TRUE(ulam_is_var_free_in(&xy, X));
    ASSERT_TRUE(ulam_is_var_free_in(&xy, Y));

    Lterm lx_x = Labs(X, &x);
    Lterm applx_x__x = Lapp(&lx_x, &x);
    ASSERT_FALSE(ulam_is_var_free_in(&applx_x__x, Y));
    ASSERT_TRUE(ulam_is_var_free_in(&applx_x__x, X));

    Lterm ly_x = Labs(Y, &x);
    Lterm applx_x__lx_y = Lapp(&lx_x, &ly_x);
    ASSERT_FALSE(ulam_is_var_free_in(&applx_x__lx_y, Y));
    ASSERT_TRUE(ulam_is_var_free_in(&applx_x__lx_y, X));

    Lterm ap1ap0lx_x_0lx_y_1y = Lapp(&applx_x__x, &y);
    ASSERT_FALSE(ulam_is_var_free_in(&ap1ap0lx_x_0lx_y_1y , FRESH_VAR));
    ASSERT_TRUE(ulam_is_var_free_in(&ap1ap0lx_x_0lx_y_1y , X));
    ASSERT_TRUE(ulam_is_var_free_in(&ap1ap0lx_x_0lx_y_1y , Y));
}

UTEST(reserved_char_count, A) {
    Lterm x = Lvar(RES_CHAR1);
    Lterm x4 = Lvar(RES_CHAR4);
    ASSERT_EQ(ulam_max_reserved_var_len(&x), 1); 
    ASSERT_EQ(ulam_max_reserved_var_len(&x4), 4); 

    Lterm lx_x4 = Labs(RES_CHAR4, &x4);
    ASSERT_EQ(ulam_max_reserved_var_len(&lx_x4), 4); 

    Lterm x4x = Lapp(&x4, &x);
    ASSERT_EQ(ulam_max_reserved_var_len(&x4x), 4); 

    Lstr fresh2 = ulam_get_fresh_var_name(&x);
    ASSERT_STREQ(ulam_str_to_cstr(fresh2), "##");

    Lstr fresh5 = ulam_get_fresh_var_name(&x4);
    ASSERT_STREQ(ulam_str_to_cstr(fresh5), "#####");

    ulam_str_free(fresh2);
    ulam_str_free(fresh5);
}

UTEST(rename, A) {
    Lterm t = Lvar(X);
    ulam_rename_var(&t, X, Y);
    match(t) {
        of(Lvar, name) { ASSERT_STREQ("y", ulam_str_to_cstr(*name)); }
        otherwise{}
    }

    Lterm y = Lvar(Y);
    Lterm ly_y = Labs(Y, &y);
    ulam_rename_var(&ly_y, Y, Z);
    match(ly_y) {
        of(Labs, x, b) {
            ASSERT_STREQ("z", ulam_str_to_cstr(*x));
            match(**b) {
                of(Lvar, name) { ASSERT_STREQ("z", ulam_str_to_cstr(*name)); }
                otherwise{}
            }
        }
        otherwise{}
    }

    Lterm y2 = Lvar(Y);
    Lterm ly_y2 = Labs(Y, &y2);
    Lterm app = Lapp(&y2, &ly_y2);
    ulam_rename_var(&app, Y, T);
    match(app) {
        of(Lapp, f, p) {
            match(**f) {
                of(Lvar, name) { ASSERT_STREQ("t", ulam_str_to_cstr(*name)); }
                otherwise{ ASSERT_TRUE(0);}
            }
            match(**p) {
                of(Labs, v, _) { ASSERT_STREQ("t", ulam_str_to_cstr(*v)); }
                otherwise{ ASSERT_TRUE(0);}
            }
        }
        otherwise{ ASSERT_TRUE(0);}
    }

    match(app) {
        of(Lapp, _, p) {
            match(**p) {
                of(Labs, _, b) { ulam_rename_var(*b, T, U); }
                otherwise{ ASSERT_TRUE(0);}
            }
        }
        otherwise{ ASSERT_TRUE(0);}
    }
    ulam_rename_var(&app,T, V);

    match(app) {
        of(Lapp, _, p) {
            match(**p) {
                of(Labs, _, b) {
                    match(**b) {
                        of(Lvar, name) {
                            ASSERT_STREQ(ulam_str_to_cstr(*name), "u");
                        }
                        otherwise{ ASSERT_TRUE(0);}
                    }
                }
                otherwise{ ASSERT_TRUE(0);}
            }
        }
        otherwise{ ASSERT_TRUE(0);}
    }
}

UTEST(lam_clone, A) {
    Lterm x = Lvar(X);
    const Lterm* x2 = ulam_clone(&x);
    ASSERT_FALSE(&x == x2);
    ASSERT_STREQ("Variable", ulam_get_form_name_cstr(x2));
    ASSERT_TRUE(ulam_are_identical(&x, x2));

    Lterm lx_x = Labs(X, &x);
    const Lterm* lx_x2 = ulam_clone(&lx_x);
    ASSERT_FALSE(&lx_x == lx_x2);
    ASSERT_STREQ( "Abstraction", ulam_get_form_name_cstr(lx_x2));
    ASSERT_TRUE(ulam_are_identical(&lx_x, lx_x2));


    Lterm y = Lvar(Y);
    Lterm applx_x__x = Lapp(&lx_x, &x);
    Lterm ap0ap1lx_x_1x_2y = Lapp(&applx_x__x, &y);
    const Lterm* ap0ap1lx_x_1x_2yB = ulam_clone(&ap0ap1lx_x_1x_2y); 
    ASSERT_FALSE(&ap0ap1lx_x_1x_2y == ap0ap1lx_x_1x_2yB);
    ASSERT_STREQ("Application", ulam_get_form_name_cstr(ap0ap1lx_x_1x_2yB));
    ASSERT_TRUE(ulam_are_identical(&ap0ap1lx_x_1x_2y, ap0ap1lx_x_1x_2yB));

}


UTEST(substitute, base_unchanged) {
    Lterm x = Lvar(X);
    Lterm s = Lvar(Y);
    Lterm* unchanged_var = ulam_substitute(&x, Y, &s);
    ASSERT_TRUE(ulam_are_identical(&x , unchanged_var));

    Lterm lx_x = Labs(X, &x);
    Lterm* unchanged_abs = ulam_substitute(&lx_x, Y, &s);
    ASSERT_TRUE(ulam_are_identical(&lx_x , unchanged_abs));

    Lterm y = Lvar(Y);
    Lterm lx_y = Labs(X, &y);
    Lterm z = Lvar(Z);
    Lterm* changed_abs = ulam_substitute(&lx_y, Y, &z);
    ASSERT_FALSE(ulam_are_identical(&lx_y , changed_abs));

    Lterm xy = Lapp(&x, &y);
    Lterm* unchanged_app = ulam_substitute(&xy, Y, &s);
    ASSERT_TRUE(ulam_are_identical(&xy , unchanged_app));
    Lterm* changed_app = ulam_substitute(&xy, X, &s);
    ASSERT_FALSE(ulam_are_identical(&xy , changed_app));

    ulam_free_term(unchanged_app);
    ulam_free_term(changed_app);
    ulam_free_term(unchanged_var);
    ulam_free_term(unchanged_abs);
    ulam_free_term(changed_abs);
}

UTEST(substitute, A) {
    Lterm x = Lvar(X);
    Lterm ly_x = Labs(Y, &x);
    Lterm lx_ly_x = Labs(X, &ly_x);
    Lterm* substituted = ulam_substitute(&x, X, &lx_ly_x);
    ASSERT_TRUE(ulam_are_identical(substituted, &lx_ly_x));


    Lterm y = Lvar(Y);
    ASSERT_FALSE(ulam_are_identical(&y, &lx_ly_x));

    Lterm lx_x = Labs(X, &x);
    Lterm applx_x__ly_x = Lapp(&lx_x, &ly_x);
    Lterm* changed = ulam_substitute(&applx_x__ly_x, X, &lx_ly_x);
    ASSERT_FALSE(ulam_are_identical(&applx_x__ly_x, changed));

    Lterm* unchanged = ulam_substitute(&applx_x__ly_x, Y, &lx_ly_x);
    ASSERT_TRUE(ulam_are_identical(&applx_x__ly_x , unchanged));

    ulam_free_term(substituted);
    ulam_free_term(changed);
    ulam_free_term(unchanged);

}


