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

Lat make_xx() { return lam_make_app(make_x(), make_x()); };
Lat make_xy() { return lam_make_app(make_x(), make_y()); };

Lat make_Llx_xRx() { return lam_make_app(make_lx_x(), make_x()); }
Lat make_Llx_xRly_x() { return lam_make_app(make_lx_x(), make_ly_x()); }
Lat make_LLlx_xRxRy() { return lam_make_app(make_Llx_xRx(), make_y()); }

UTEST_MAIN()

UTEST(term_form_name,A) {
    Lat x = make_x();
    ASSERT_STREQ(lam_term_form_name(x), "Variable");

    Lat lx_x = make_lx_x();
    ASSERT_STREQ(lam_term_form_name(lx_x), "Abstraction");

    Lat Llx_xRx = make_Llx_xRx ();
    ASSERT_STREQ(lam_term_form_name(Llx_xRx), "Application");
    lam_free(Llx_xRx);
}



UTEST(lam_free_vars, Abs) {

    Lat lx_x = make_lx_x();
    ASSERT_FALSE(is_var_free_in(lx_x, "x"));
    ASSERT_FALSE(is_var_free_in(lx_x, "fresh var"));


    Lat ly_x = make_ly_x();
    ASSERT_TRUE (is_var_free_in(ly_x, "x"));

    Lat lz_ly_x = make_lz_ly_x();
    ASSERT_TRUE(is_var_free_in(lz_ly_x, "x"));
    ASSERT_FALSE(is_var_free_in(lz_ly_x, "y"));
    ASSERT_FALSE(is_var_free_in(lz_ly_x, "z"));

    Lat lz_lx_x = make_lz_lx_x();
    ASSERT_FALSE(is_var_free_in(lz_lx_x, "x"));
    ASSERT_FALSE(is_var_free_in(lz_lx_x, "y"));
    ASSERT_FALSE(is_var_free_in(lz_lx_x, "z"));

    Lat lx_ly_x = make_lx_ly_x();
    ASSERT_FALSE(is_var_free_in(lx_ly_x, "x"));
    ASSERT_FALSE(is_var_free_in(lx_ly_x, "y"));
    ASSERT_FALSE(is_var_free_in(lx_ly_x, "z"));

    lam_free(lx_x);
    lam_free(ly_x);
    lam_free(lz_ly_x);
    lam_free(lz_lx_x);
    lam_free(lx_ly_x);
}


UTEST(LamTermsFixture, fre_vars_app) {
    Lat xx = make_xx();
    ASSERT_FALSE(is_var_free_in(xx, "y"));
    ASSERT_TRUE(is_var_free_in(xx, "x"));

    Lat xy = make_xy();

    ASSERT_FALSE(is_var_free_in(xy, "fresh var"));
    ASSERT_TRUE(is_var_free_in(xy, "x"));
    ASSERT_TRUE(is_var_free_in(xy, "y"));

    Lat Llx_xRx = make_Llx_xRx();
    ASSERT_FALSE(is_var_free_in(Llx_xRx, "y"));
    ASSERT_TRUE(is_var_free_in(Llx_xRx, "x"));

    Lat Llx_xRly_x = make_Llx_xRly_x();
    ASSERT_FALSE(is_var_free_in(Llx_xRly_x, "y"));
    ASSERT_TRUE(is_var_free_in(Llx_xRly_x, "x"));

    Lat LLlx_xRxRy = make_LLlx_xRxRy();
    ASSERT_FALSE(is_var_free_in(LLlx_xRxRy, "fresh var"));
    ASSERT_TRUE(is_var_free_in(LLlx_xRxRy, "x"));
    ASSERT_TRUE(is_var_free_in(LLlx_xRxRy, "y"));

    lam_free(xx);
    lam_free(xy);
    lam_free(Llx_xRx);
    lam_free(Llx_xRly_x);
    lam_free(LLlx_xRxRy);
}

UTEST(reserved_char_count, A) {
    Lat x = lam_make_var("#");
    Lat x4 = lam_make_var("####");
    ASSERT_EQ(max_reserved_var_len(x), 1); 
    ASSERT_EQ(max_reserved_var_len(x4), 4); 

    Lat lx_x4 = lam_make_abs("####", lam_make_var("####"));
    ASSERT_EQ(max_reserved_var_len(lx_x4), 4); 

    Lat x4x = lam_make_app(lam_make_var("####"), lam_make_var("#"));
    ASSERT_EQ(max_reserved_var_len(x4x), 4); 

    Lstr fresh2 = get_fresh_var_name(x);
    ASSERT_STREQ(fresh2, "##");

    Lstr fresh5 = get_fresh_var_name(x4);
    ASSERT_STREQ(fresh5, "#####");

    lam_free(x);
    lam_free(x4);
    lam_free(lx_x4);
    lam_free(x4x);
    free((char*)fresh2);
    free((char*)fresh5);
}

UTEST(rename, A) {
    Lat t = make_x();
    lam_rename_var(t, "x", "y");
    ASSERT_STREQ("y", lam_get_var_name(t));
    lam_free(t);

    LatAbs* ly_y = make_ly_y();
    lam_rename_var(ly_y, "y", "z");
    ASSERT_STREQ("z", lam_get_abs_var_name(ly_y));
    ASSERT_STREQ("z", lam_get_var_name(ly_y->body));
    lam_free(ly_y);

    LatApp* app = lam_make_app(make_y(), make_ly_y());
    lam_rename_var(app, "y", "t");
    ASSERT_STREQ("t", lam_get_var_name(app->fun));
    ASSERT_STREQ("t", lam_get_abs_var_name(app->param));

    lam_rename_var(((LatAbs*)app->param)->body, "t", "u");
    lam_rename_var(app, "t", "v");
    ASSERT_STREQ(lam_get_var_name(((LatAbs*)app->param)->body), "u");
    lam_free(app);
}

//TODO: implement lam_is_identical and assert with it
UTEST(lam_clone, A) {
    Lat x = make_x();
    Lat x2 = lam_clone(x);
    ASSERT_FALSE(x == x2);
    ASSERT_STREQ("Variable", lam_term_form_name(x2));
    ASSERT_TRUE(lam_are_identical(x, x2));

    Lat lx_x = make_lx_x();
    Lat lx_x2 = lam_clone(lx_x);
    ASSERT_FALSE(lx_x == lx_x2);
    ASSERT_STREQ("Abstraction", lam_term_form_name(lx_x2));
    ASSERT_TRUE(lam_are_identical(lx_x, lx_x2));

    Lat LLlx_xRxRy = make_LLlx_xRxRy();
    Lat LLlx_xRxRy2 = lam_clone(LLlx_xRxRy);
    ASSERT_FALSE(LLlx_xRxRy == LLlx_xRxRy2);
    ASSERT_STREQ("Application", lam_term_form_name(LLlx_xRxRy2));
    ASSERT_TRUE(lam_are_identical(LLlx_xRxRy, LLlx_xRxRy2));

    lam_free(x);
    lam_free(x2);
    lam_free(lx_x);
    lam_free(lx_x2);
    lam_free(LLlx_xRxRy);
    lam_free(LLlx_xRxRy2);
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


UTEST(to_string, A) {
    Lat x = make_x();
    Lstr x_str = lam_to_string(x);
    ASSERT_STREQ("x", x_str);

    Lat lx_x = make_lx_x();
    Lstr str_lx_x = lam_to_string(lx_x);
    ASSERT_STREQ("(\\x.x)", str_lx_x);

    Lat lx_ly_x = make_lx_ly_x();
    Lstr str_lx_ly_x = lam_to_string(lx_ly_x);
    ASSERT_STREQ("(\\x.(\\y.x))", str_lx_ly_x);

    Lat Llx_xRly_x  = make_Llx_xRly_x ();
    Lstr str_Llx_xRly_x  = lam_to_string(Llx_xRly_x );
    ASSERT_STREQ("((\\x.x)(\\y.x))", str_Llx_xRly_x );
}
