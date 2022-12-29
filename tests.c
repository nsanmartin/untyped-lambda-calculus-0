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
