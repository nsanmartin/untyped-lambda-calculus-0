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


#define ASSERT_LTERM_NOTNULL(PTR)                                       \
        ASSERT_NE((intptr_t)PTR, 0);

#define ASSERT_LTERM_EQ_STR(STR, LTERM)                                 \
    do {                                                                \
        char* tstr;                                                     \
        tstr = lam_term_to_string(&LTERM);                              \
        ASSERT_NE((intptr_t)tstr, 0);                                   \
        ASSERT_LTERM_NOTNULL(tstr)                                      \
        ASSERT_STREQ(STR, tstr);                                        \
        free(tstr);                                                     \
    } while (0);

#define ASSERT_LVAR(T)                                                  \
    ASSERT_EQ((T)->tag, Lvartag)

#define ASSERT_LABS(T)                                                  \
    ASSERT_EQ((T)->tag, Labstag)

#define ASSERT_LAPP(T)                                                  \
    ASSERT_EQ((T)->tag, Lapptag)

UTEST_MAIN()

UTEST(term_form_name,A) {
    Lterm x = LVAR(X);
    ASSERT_STREQ("Variable", ulam_str_to_cstr(lam_get_form_name(&x)));

    Lterm lx_x = LABS(X, x);
    ASSERT_STREQ("Abstraction", ulam_str_to_cstr(lam_get_form_name(&lx_x)));

    Lterm app = LAPP(lx_x, x);
    ASSERT_STREQ("Application", ulam_str_to_cstr(lam_get_form_name(&app)));

    ASSERT_LTERM_EQ_STR("x", x);
    ASSERT_LTERM_EQ_STR("(\\x.x)", lx_x);
    ASSERT_LTERM_EQ_STR("((\\x.x) x)", app);
}



UTEST(lam_free_vars, Abs) {

    Lterm x = LVAR(X);
    Lterm lx_x = LABS(X, x);
    ASSERT_FALSE(lam_is_var_free_in(&lx_x, X));
    ASSERT_FALSE(lam_is_var_free_in(&lx_x, FRESH_VAR));

    Lterm ly_x = LABS(Y, x);
    ASSERT_TRUE (lam_is_var_free_in(&ly_x, X));

    Lterm lz_ly_x = LABS(Z, ly_x);
    ASSERT_TRUE (lam_is_var_free_in(&lz_ly_x, X));
    ASSERT_FALSE(lam_is_var_free_in(&lz_ly_x, Y));
    ASSERT_FALSE(lam_is_var_free_in(&lz_ly_x, Z));

    Lterm lz_lx_x = LABS(Z, lx_x);
    ASSERT_FALSE(lam_is_var_free_in(&lz_lx_x, X));
    ASSERT_FALSE(lam_is_var_free_in(&lz_lx_x, Y));
    ASSERT_FALSE(lam_is_var_free_in(&lz_lx_x, Z));

    Lterm lx_ly_x = LABS(X, ly_x);
    ASSERT_FALSE(lam_is_var_free_in(&lx_ly_x, X));
    ASSERT_FALSE(lam_is_var_free_in(&lx_ly_x, Y));
    ASSERT_FALSE(lam_is_var_free_in(&lx_ly_x, Z));

    ASSERT_LTERM_EQ_STR("(\\y.x)", ly_x);
    ASSERT_LTERM_EQ_STR("(\\z.(\\y.x))", lz_ly_x);
    ASSERT_LTERM_EQ_STR("(\\z.(\\x.x))", lz_lx_x);
    ASSERT_LTERM_EQ_STR("(\\x.(\\y.x))", lx_ly_x);
}


UTEST(LtermsFixture, free_vars_app) {

    Lterm  x = LVAR(X);
    Lterm  xx = LAPP(x, x);
    ASSERT_FALSE(lam_is_var_free_in(&xx, Y));
    ASSERT_TRUE(lam_is_var_free_in(&xx, X));

    Lterm y = LVAR(Y);
    Lterm xy = LAPP(x, y);

    ASSERT_FALSE(lam_is_var_free_in(&xy, FRESH_VAR));
    ASSERT_TRUE(lam_is_var_free_in(&xy, X));
    ASSERT_TRUE(lam_is_var_free_in(&xy, Y));

    Lterm lx_x = LABS(X, x);
    Lterm applx_x__x = LAPP(lx_x, x);
    ASSERT_FALSE(lam_is_var_free_in(&applx_x__x, Y));
    ASSERT_TRUE(lam_is_var_free_in(&applx_x__x, X));

    Lterm ly_x = LABS(Y, x);
    Lterm applx_x__ly_x = LAPP(lx_x, ly_x);
    ASSERT_FALSE(lam_is_var_free_in(&applx_x__ly_x, Y));
    ASSERT_TRUE(lam_is_var_free_in(&applx_x__ly_x, X));

    Lterm ap1ap0lx_x_0lx_y_1y = LAPP(applx_x__x, y);
    ASSERT_FALSE(lam_is_var_free_in(&ap1ap0lx_x_0lx_y_1y , FRESH_VAR));
    ASSERT_TRUE(lam_is_var_free_in(&ap1ap0lx_x_0lx_y_1y , X));
    ASSERT_TRUE(lam_is_var_free_in(&ap1ap0lx_x_0lx_y_1y , Y));


    ASSERT_LTERM_EQ_STR("(x x)", xx);
    ASSERT_LTERM_EQ_STR("(x y)", xy);
    ASSERT_LTERM_EQ_STR("((\\x.x) (\\y.x))", applx_x__ly_x);
    ASSERT_LTERM_EQ_STR("(((\\x.x) x) y)", ap1ap0lx_x_0lx_y_1y);

}

UTEST(reserved_char_count, A) {
    Lterm x = LVAR(RES_CHAR1);
    Lterm x4 = LVAR(RES_CHAR4);
    ASSERT_EQ(lam_max_reserved_var_len(&x), 1); 
    ASSERT_EQ(lam_max_reserved_var_len(&x4), 4); 

    Lterm lx_x4 = LABS(RES_CHAR4, x4);
    ASSERT_EQ(lam_max_reserved_var_len(&lx_x4), 4); 

    Lterm x4x = LAPP(x4, x);
    ASSERT_EQ(lam_max_reserved_var_len(&x4x), 4); 

    Lstr fresh2 = lam_get_fresh_var_name(&x);
    ASSERT_STREQ(ulam_str_to_cstr(fresh2), "##");

    Lstr fresh5 = lam_get_fresh_var_name(&x4);
    ASSERT_STREQ(ulam_str_to_cstr(fresh5), "#####");

    ulam_str_free(fresh2);
    ulam_str_free(fresh5);

    ASSERT_LTERM_EQ_STR("#", x);
    ASSERT_LTERM_EQ_STR("####", x4);
    ASSERT_LTERM_EQ_STR("(\\####.####)", lx_x4);
    ASSERT_LTERM_EQ_STR("(#### #)", x4x);

}

UTEST(rename, A) {
    /// x, y
    Lterm t = LVAR(X);
    ASSERT_LTERM_EQ_STR("x", t);
    lam_rename_var(&t, X, Y);
    ASSERT_LTERM_EQ_STR("y", t);
    ASSERT_LVAR(&t);
    ASSERT_STREQ("y", ulam_str_to_cstr(t.var.name));

    /// y, \y.y, \z.z
    Lterm ly_y = LABS(Y, LVAR(Y));
    ASSERT_LTERM_EQ_STR("(\\y.y)", ly_y);
    lam_rename_var(&ly_y, Y, Z);
    ASSERT_LTERM_EQ_STR("(\\z.z)", ly_y);
    ASSERT_LABS(&ly_y);
    ASSERT_STREQ("z", ulam_str_to_cstr(ly_y.abs.vname));
    ASSERT_LVAR(ly_y.abs.body);
    ASSERT_STREQ("z", ulam_str_to_cstr(ly_y.abs.body->var.name));

    /// y, \y.y, (y \y.y)
    Lterm app = LAPP(LVAR(Y), LABS(Y, LVAR(Y)));
    ASSERT_LTERM_EQ_STR("y", LVAR(Y));
    ASSERT_LTERM_EQ_STR("(\\y.y)", LABS(Y, LVAR(Y)));
    ASSERT_LTERM_EQ_STR("(y (\\y.y))", app);
    lam_rename_var(&app, Y, T);
    ASSERT_LTERM_EQ_STR("(t (\\t.t))", app);

    ASSERT_LVAR(app.app.fun);
    ASSERT_STREQ("t", ulam_str_to_cstr(app.app.fun->var.name));
    ASSERT_LABS(app.app.param);
    ASSERT_STREQ("t", ulam_str_to_cstr(app.app.param->abs.vname));
    ASSERT_LTERM_EQ_STR("(t (\\t.t))", app);
    ASSERT_LVAR(app.app.param->abs.body);
    Lterm* body = app.app.param->abs.body;
    lam_rename_var(body, T, U);
    ASSERT_LTERM_EQ_STR("(t (\\t.u))", app);
    lam_rename_var(&app,T, V);
    ASSERT_LTERM_EQ_STR("(v (\\v.u))", app);
}

UTEST(lam_clone, A) {
    //Lterm x = LVAR(X);
    const Lterm* x2 = lam_clone(&LVAR(X));
    ASSERT_FALSE(&LVAR(X) == x2);
    ASSERT_STREQ("Variable", lam_get_form_name_cstr(x2));
    ASSERT_TRUE(lam_are_identical(&LVAR(X), x2));

    Lterm lx_x = LABS(X, LVAR(X));
    const Lterm* lx_x2 = lam_clone(&lx_x);
    ASSERT_FALSE(&lx_x == lx_x2);
    ASSERT_STREQ( "Abstraction", lam_get_form_name_cstr(lx_x2));
    ASSERT_TRUE(lam_are_identical(&lx_x, lx_x2));


    Lterm y = LVAR(Y);
    Lterm applx_x__x = LAPP(lx_x, LVAR(X));
    Lterm ap0ap1lx_x_1x_2y = LAPP(applx_x__x, y);
    const Lterm* ap0ap1lx_x_1x_2yB = lam_clone(&ap0ap1lx_x_1x_2y); 
    ASSERT_FALSE(&ap0ap1lx_x_1x_2y == ap0ap1lx_x_1x_2yB);
    ASSERT_STREQ("Application", lam_get_form_name_cstr(ap0ap1lx_x_1x_2yB));
    ASSERT_TRUE(lam_are_identical(&ap0ap1lx_x_1x_2y, ap0ap1lx_x_1x_2yB));

    lam_free_term((Lterm*)x2);
    lam_free_term((Lterm*)ap0ap1lx_x_1x_2yB);
    lam_free_term((Lterm*)lx_x2);
}



UTEST(substitute, base_unchanged) {
    Lterm x = LVAR(X);
    Lterm s = LVAR(Y);
    Lterm* unchanged_var = lam_substitute(&x, Y, &s);
    ASSERT_TRUE(lam_are_identical(&x , unchanged_var));

    Lterm lx_x = LABS(X, LVAR(X));
    Lterm* unchanged_abs = lam_substitute(&lx_x, Y, &s);
    ASSERT_TRUE(lam_are_identical(&lx_x , unchanged_abs));

    Lterm lx_y = LABS(X, LVAR(Y));
    Lterm z = LVAR(Z);
    Lterm* changed_abs = lam_substitute(&lx_y, Y, &z);
    ASSERT_FALSE(lam_are_identical(&lx_y , changed_abs));

    Lterm xy = LAPP(LVAR(X), LVAR(Y));
    Lterm* unchanged_app = lam_substitute(&xy, Y, &s);
    ASSERT_TRUE(lam_are_identical(&xy , unchanged_app));
    Lterm* changed_app = lam_substitute(&xy, X, &s);
    ASSERT_FALSE(lam_are_identical(&xy , changed_app));

    lam_free_term(unchanged_app);
    lam_free_term(changed_app);
    lam_free_term(unchanged_var);
    lam_free_term(unchanged_abs);
    lam_free_term(changed_abs);
}

UTEST(substitute, A) {
    Lterm x = LVAR(X);
    Lterm ly_x = LABS(Y, LVAR(X));
    Lterm lx_ly_x = LABS(X, ly_x);
    Lterm* substituted = lam_substitute(&x, X, &lx_ly_x);
    ASSERT_TRUE(lam_are_identical(substituted, &lx_ly_x));


    ASSERT_FALSE(lam_are_identical(&LVAR(Y), &lx_ly_x));

    Lterm lx_x = LABS(X, LVAR(X));
    Lterm applx_x__ly_x = LAPP(lx_x, ly_x);
    Lterm* changed = lam_substitute(&applx_x__ly_x, X, &lx_ly_x);
    ASSERT_FALSE(lam_are_identical(&applx_x__ly_x, changed));

    Lterm* unchanged = lam_substitute(&applx_x__ly_x, Y, &lx_ly_x);
    ASSERT_TRUE(lam_are_identical(&applx_x__ly_x , unchanged));

    lam_free_term(substituted);
    lam_free_term(changed);
    lam_free_term(unchanged);

}


