#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "utest.h"

#include "lam.h"

UTEST_MAIN()

UTEST(term_form, A) {
    Lat x = lam_make_var("x");
    Lat lx_x = lam_make_abs("x", x);
    Lat lx_xx = lam_make_app(lx_x, x);
    
    ASSERT_STREQ(lam_term_form_name(x), "Variable");
    ASSERT_STREQ(lam_term_form_name(lx_x), "Abstraction");
    ASSERT_STREQ(lam_term_form_name(lx_xx), "Application");
}

