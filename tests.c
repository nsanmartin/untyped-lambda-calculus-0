#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "utest.h"

#include "lam.h"

struct LamTermsFixture {
    // vars
    Lat x, y;
    // abstractions
    Lat lx_x, ly_x, lz_ly_x, lz_lx_x, lx_ly_x;
    //applications
    Lat xx, xy, Llx_xRx, Llx_xRly_x, LLlx_xRxRy;
};

UTEST_F_SETUP(LamTermsFixture) {
    utest_fixture->x = lam_make_var("x");
    utest_fixture->y = lam_make_var("y");

    utest_fixture->lx_x = lam_make_abs("x", utest_fixture->x);
    utest_fixture->ly_x = lam_make_abs("y", utest_fixture->x);
    utest_fixture->lz_ly_x = lam_make_abs("z", utest_fixture->ly_x);
    utest_fixture->lz_lx_x = lam_make_abs("z", utest_fixture->lx_x);
    utest_fixture->lx_ly_x = lam_make_abs("x", utest_fixture->ly_x);

    utest_fixture->xx = lam_make_app(utest_fixture->x, utest_fixture->x);
    utest_fixture->xy = lam_make_app(utest_fixture->x, utest_fixture->y);
    utest_fixture->Llx_xRx = lam_make_app(utest_fixture->lx_x, utest_fixture->x);
    utest_fixture->Llx_xRly_x = lam_make_app(utest_fixture->lx_x, utest_fixture->ly_x);
    utest_fixture->LLlx_xRxRy = lam_make_app(utest_fixture->Llx_xRx, utest_fixture->y);
}

UTEST_F_TEARDOWN(LamTermsFixture) {
    lam_free(utest_fixture->x);
    lam_free(utest_fixture->y);

    lam_free(utest_fixture->lx_x);
    lam_free(utest_fixture->Llx_xRx);
    lam_free(utest_fixture->ly_x);
    lam_free(utest_fixture->lz_ly_x);
    lam_free(utest_fixture->lz_lx_x);
    lam_free(utest_fixture->lx_ly_x);

    lam_free(utest_fixture->xx);
    lam_free(utest_fixture->xy);
    lam_free(utest_fixture->Llx_xRx);
    lam_free(utest_fixture->Llx_xRly_x);
    lam_free(utest_fixture-> LLlx_xRxRy);
}

UTEST_MAIN()

UTEST_F(LamTermsFixture, term_form_name) {
    ASSERT_STREQ(lam_term_form_name(utest_fixture->x), "Variable");
    ASSERT_STREQ(lam_term_form_name(utest_fixture->lx_x), "Abstraction");
    ASSERT_STREQ(lam_term_form_name(utest_fixture->Llx_xRx), "Application");
}


UTEST_F(LamTermsFixture, free_vars_var) {
    ASSERT_TRUE(is_var_free_in(utest_fixture->x, "x"));
    ASSERT_FALSE(is_var_free_in(utest_fixture->x, "fresh var"));
}

UTEST_F(LamTermsFixture, free_vars_abs) {

    ASSERT_FALSE(is_var_free_in(utest_fixture->lx_x, "x"));
    ASSERT_TRUE (is_var_free_in(utest_fixture->ly_x, "x"));
    ASSERT_FALSE(is_var_free_in(utest_fixture->lx_x, "fresh var"));

    ASSERT_TRUE(is_var_free_in(utest_fixture->lz_ly_x, "x"));
    ASSERT_FALSE(is_var_free_in(utest_fixture->lz_ly_x, "y"));
    ASSERT_FALSE(is_var_free_in(utest_fixture->lz_ly_x, "z"));

    ASSERT_FALSE(is_var_free_in(utest_fixture->lz_lx_x, "x"));
    ASSERT_FALSE(is_var_free_in(utest_fixture->lz_lx_x, "y"));
    ASSERT_FALSE(is_var_free_in(utest_fixture->lz_lx_x, "z"));

    ASSERT_FALSE(is_var_free_in(utest_fixture->lx_ly_x, "x"));
    ASSERT_FALSE(is_var_free_in(utest_fixture->lx_ly_x, "y"));
    ASSERT_FALSE(is_var_free_in(utest_fixture->lx_ly_x, "z"));

}


UTEST_F(LamTermsFixture, fre_vars_app) {
    ASSERT_FALSE(is_var_free_in(utest_fixture->xx, "y"));
    ASSERT_TRUE(is_var_free_in(utest_fixture->xx, "x"));

    ASSERT_FALSE(is_var_free_in(utest_fixture->xy, "fresh var"));
    ASSERT_TRUE(is_var_free_in(utest_fixture->xy, "x"));
    ASSERT_TRUE(is_var_free_in(utest_fixture->xy, "y"));

    ASSERT_FALSE(is_var_free_in(utest_fixture->Llx_xRx, "y"));
    ASSERT_TRUE(is_var_free_in(utest_fixture->Llx_xRx, "x"));

    ASSERT_FALSE(is_var_free_in(utest_fixture->Llx_xRly_x, "y"));
    ASSERT_TRUE(is_var_free_in(utest_fixture->Llx_xRly_x, "x"));

    ASSERT_FALSE(is_var_free_in(utest_fixture->LLlx_xRxRy, "fresh var"));
    ASSERT_TRUE(is_var_free_in(utest_fixture->LLlx_xRxRy, "x"));
    ASSERT_TRUE(is_var_free_in(utest_fixture->LLlx_xRxRy, "y"));

}

UTEST(reserved_char_count, A) {
    Lat x = lam_make_var("#");
    Lat x4 = lam_make_var("####");
    ASSERT_EQ(max_reserved_var_len(x), 1); 
    ASSERT_EQ(max_reserved_var_len(x4), 4); 

    Lat lx_x4 = lam_make_abs("####", x4);
    ASSERT_EQ(max_reserved_var_len(lx_x4), 4); 

    Lat x4x = lam_make_app(x4, x);
    ASSERT_EQ(max_reserved_var_len(x4x), 4); 

    Lstr fresh2 = get_fresh_var_name(x);
    ASSERT_STREQ(fresh2, "##");

    Lstr fresh5 = get_fresh_var_name(x4);
    ASSERT_STREQ(fresh5, "#####");
    //TODO: free x, x4, fresh2, fresh5.
}

UTEST(rename, A) {
    Lat t = lam_make_var("x");
    lam_rename_var(t, "x", "y");
    ASSERT_STREQ("y", lam_get_var_name(t));

    LatAbs* ly_y = lam_make_abs("y", t);
    lam_rename_var(ly_y, "y", "z");
    ASSERT_STREQ("z", lam_get_abs_var_name(ly_y));
    ASSERT_STREQ("z", lam_get_var_name(ly_y->body));

    Lat z = lam_make_var("z");
    LatApp* app = lam_make_app(z, ly_y);
    lam_rename_var(app, "z", "t");
    ASSERT_STREQ("t", lam_get_var_name(app->fun));
    ASSERT_STREQ("t", lam_get_abs_var_name(app->param));

    lam_rename_var(((LatAbs*)app->param)->body, "t", "u");
    lam_rename_var(app, "t", "v");
    ASSERT_STREQ(lam_get_var_name(((LatAbs*)app->param)->body), "u");
    //TODO: free all
}

