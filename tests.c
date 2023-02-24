#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "utest.h"

#include "lam.h"

Lat make_x() { return lam_make_var("x"); }
Lat make_y() { return lam_make_var("y"); }
Lat make_z() { return lam_make_var("z"); }
Lat make_lx_x() { return lam_make_abs("x", make_x()); }
Lat make_ly_x() { return lam_make_abs("y", make_x()); }
Lat make_ly_y() { return lam_make_abs("y", make_y()); }
Lat make_lz_ly_x() { return lam_make_abs("z", make_ly_x()); } 
Lat make_lz_lx_x() { return lam_make_abs("z", make_lx_x()); } 
Lat make_lx_ly_x() { return lam_make_abs("x", make_ly_x()); } 

Lat make_xx() { return lam_make_app(make_x(), make_x()); }
Lat make_xy() { return lam_make_app(make_x(), make_y()); }

Lat make_Llx_xRx() { return lam_make_app(make_lx_x(), make_x()); }
Lat make_Llx_xRly_x() { return lam_make_app(make_lx_x(), make_ly_x()); }
Lat make_LLlx_xRxRy() { return lam_make_app(make_Llx_xRx(), make_y()); }

UTEST_MAIN()

UTEST(term_form_name,A) {
    Lterm x = Lvar("x");
    ASSERT_STREQ(ulam_get_form_name(&x), "Variable");

    Lterm lx_x = Labs("x", &x);
    ASSERT_STREQ(ulam_get_form_name(&lx_x), "Abstraction");

    Lterm applx_x__x = Lapp(&lx_x, &x);
    ASSERT_STREQ(ulam_get_form_name(&applx_x__x), "Application");
}



UTEST(lam_free_vars, Abs) {

    Lterm x = Lvar("x");
    Lterm lx_x = Labs("x", &x);
    ASSERT_FALSE(ulam_is_var_free_in(&lx_x, "x"));
    ASSERT_FALSE(ulam_is_var_free_in(&lx_x, "fresh var"));


    Lterm ly_x = Labs("y", &x);
    ASSERT_TRUE (ulam_is_var_free_in(&ly_x, "x"));

    Lterm lz_ly_x = Labs("z", &ly_x);
    ASSERT_TRUE(ulam_is_var_free_in( &lz_ly_x, "x"));
    ASSERT_FALSE(ulam_is_var_free_in(&lz_ly_x, "y"));
    ASSERT_FALSE(ulam_is_var_free_in(&lz_ly_x, "z"));

    Lterm lz_lx_x = Labs("z", &lx_x);
    ASSERT_FALSE(ulam_is_var_free_in(&lz_lx_x, "x"));
    ASSERT_FALSE(ulam_is_var_free_in(&lz_lx_x, "y"));
    ASSERT_FALSE(ulam_is_var_free_in(&lz_lx_x, "z"));

    Lterm lx_ly_x = Labs("x", &ly_x);
    ASSERT_FALSE(ulam_is_var_free_in(&lx_ly_x, "x"));
    ASSERT_FALSE(ulam_is_var_free_in(&lx_ly_x, "y"));
    ASSERT_FALSE(ulam_is_var_free_in(&lx_ly_x, "z"));
}


UTEST(LamTermsFixture, free_vars_app) {
    Lterm x = Lvar("x");
    Lterm xx = Lapp(&x, &x);
    ASSERT_FALSE(ulam_is_var_free_in(&xx, "y"));
    ASSERT_TRUE(ulam_is_var_free_in(&xx, "x"));

    Lterm y = Lvar("y");
    Lterm xy = Lapp(&x, &y);

    ASSERT_FALSE(ulam_is_var_free_in(&xy, "fresh var"));
    ASSERT_TRUE(ulam_is_var_free_in(&xy, "x"));
    ASSERT_TRUE(ulam_is_var_free_in(&xy, "y"));

    Lterm lx_x = Labs("x", &x);
    Lterm applx_x__x = Lapp(&lx_x, &x);
    ASSERT_FALSE(ulam_is_var_free_in(&applx_x__x, "y"));
    ASSERT_TRUE(ulam_is_var_free_in(&applx_x__x, "x"));

    Lterm ly_x = Labs("y", &x);
    Lterm applx_x__lx_y = Lapp(&lx_x, &ly_x);
    ASSERT_FALSE(ulam_is_var_free_in(&applx_x__lx_y, "y"));
    ASSERT_TRUE(ulam_is_var_free_in(&applx_x__lx_y, "x"));

    Lterm ap1ap0lx_x_0lx_y_1y = Lapp(&applx_x__x, &y);
    ASSERT_FALSE(ulam_is_var_free_in(&ap1ap0lx_x_0lx_y_1y , "fresh var"));
    ASSERT_TRUE(ulam_is_var_free_in(&ap1ap0lx_x_0lx_y_1y , "x"));
    ASSERT_TRUE(ulam_is_var_free_in(&ap1ap0lx_x_0lx_y_1y , "y"));
}

UTEST(reserved_char_count, A) {
    Lterm x = Lvar("#");
    Lterm x4 = Lvar("####");
    ASSERT_EQ(ulam_max_reserved_var_len(&x), 1); 
    ASSERT_EQ(ulam_max_reserved_var_len(&x4), 4); 

    Lterm lx_x4 = Labs("####", &x4);
    ASSERT_EQ(ulam_max_reserved_var_len(&lx_x4), 4); 

    Lterm x4x = Lapp(&x4, &x);
    ASSERT_EQ(ulam_max_reserved_var_len(&x4x), 4); 

    Lstr fresh2 = ulam_get_fresh_var_name(&x);
    ASSERT_STREQ(fresh2, "##");

    Lstr fresh5 = get_fresh_var_name(&x4);
    ASSERT_STREQ(fresh5, "#####");

    free((char*)fresh2);
    free((char*)fresh5);
}

UTEST(rename, A) {
    Lterm t = Lvar("x");
    ulam_rename_var(&t, "x", "y");
    match(t) {
        of(Lvar, name) { ASSERT_STREQ("y", *name); }
        otherwise{}
    }

    Lterm y = Lvar("y");
    Lterm ly_y = Labs("y", &y);
    ulam_rename_var(&ly_y, "y", "z");
    match(ly_y) {
        of(Labs, x, b) {
            ASSERT_STREQ("z", *x);
            match(**b) {
                of(Lvar, name) { ASSERT_STREQ("z", *name); }
                otherwise{}
            }
        }
        otherwise{}
    }

    Lterm y2 = Lvar("y");
    Lterm ly_y2 = Labs("y", &y2);
    Lterm app = Lapp(&y2, &ly_y2);
    ulam_rename_var(&app, "y", "t");
    match(app) {
        of(Lapp, f, p) {
            match(**f) {
                of(Lvar, name) { ASSERT_STREQ("t", *name); }
                otherwise{ ASSERT_TRUE(0);}
            }
            match(**p) {
                of(Labs, v, _) { ASSERT_STREQ("t", *v); }
                otherwise{ ASSERT_TRUE(0);}
            }
        }
        otherwise{ ASSERT_TRUE(0);}
    }

    match(app) {
        of(Lapp, _, p) {
            match(**p) {
                of(Labs, _, b) { ulam_rename_var(*b, "t", "u"); }
                otherwise{ ASSERT_TRUE(0);}
            }
        }
        otherwise{ ASSERT_TRUE(0);}
    }
    ulam_rename_var(&app, "t", "v");

    match(app) {
        of(Lapp, _, p) {
            match(**p) {
                of(Labs, _, b) { ASSERT_STREQ(lam_get_var_name(*b), "u"); }
                otherwise{ ASSERT_TRUE(0);}
            }
        }
        otherwise{ ASSERT_TRUE(0);}
    }
}

//TODO: implement lam_is_identical and assert with it
UTEST(lam_clone, A) {
    Lterm x = Lvar("x");
    const Lterm* x2 = ulam_clone(&x);
    ASSERT_FALSE(&x == x2);
    ASSERT_STREQ("Variable", ulam_get_form_name(x2));
    ASSERT_TRUE(ulam_are_identical(&x, x2));

//    Lterm lx_x = Labs("x", &x);
//    const Lterm* lx_x2 = ulam_clone(&lx_x);
//    ASSERT_FALSE(&lx_x == lx_x2);
//    ASSERT_STREQ("Abstraction", ulam_get_form_name(lx_x2));
//    ASSERT_TRUE(ulam_are_identical(&lx_x, lx_x2));
//
//
//    Lterm y = Lvar("y");
//    Lterm applx_x__x = Lapp(&lx_x, &x);
//    Lterm ap0ap1lx_x_1x_2y = Lapp(&applx_x__x, &y);
//    const Lterm* ap0ap1lx_x_1x_2yB = ulam_clone(&ap0ap1lx_x_1x_2y); 
//    ASSERT_FALSE(&ap0ap1lx_x_1x_2y == ap0ap1lx_x_1x_2yB);
//    ASSERT_STREQ("Application", ulam_get_form_name(ap0ap1lx_x_1x_2yB));
//    ASSERT_TRUE(ulam_are_identical(&ap0ap1lx_x_1x_2y, ap0ap1lx_x_1x_2yB));

}


UTEST(substitute, base_unchanged) {
    Lat x = make_x();
    Lat s = make_y();
    Lat unchanged_var = lam_substitute(x, "Y", s);
    ASSERT_TRUE(lam_are_identical(x , unchanged_var));

    Lat lx_x = make_lx_x();
    Lat unchanged_abs = lam_substitute(lx_x, "Y", s);
    ASSERT_TRUE(lam_are_identical(lx_x , unchanged_abs));

    Lat xy = make_xy();
    Lat unchanged_app = lam_substitute(xy, "Y", s);
    ASSERT_TRUE(lam_are_identical(xy , unchanged_app));
    //todo: free
}

UTEST(substitute, A) {
    Lat x = make_x();
    Lat s = make_lx_ly_x();
    Lat substituted = lam_substitute(x, "x", s);
    ASSERT_TRUE(lam_are_identical(substituted, s));

    Lat y = make_y();
    ASSERT_FALSE(lam_are_identical(y, s));

    //Lat make_Llx_xRly_x() { return lam_make_app(make_lx_x(), make_ly_x()); }
    Lat Llx_xRly_x = make_Llx_xRly_x();
    Lat changed = lam_substitute(Llx_xRly_x, "x", s);
    ASSERT_FALSE(lam_are_identical(Llx_xRly_x , changed));

    Lat unchanged = lam_substitute(Llx_xRly_x, "Y", s);
    ASSERT_TRUE(lam_are_identical(Llx_xRly_x , unchanged));

    // lam_free(x);
    // lam_free(s);
    // lam_free(y);
}


