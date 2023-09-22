#include "parser.tab.c"
#include "util.c"

#define test_parse(S)                                                   \
    printf("parsing '%s' => ", S);                                      \
    parse_string( S "\n");

int main (void) {
    test_parse("x");
    test_parse("(x x)");
    test_parse("(\\x.(x x))");
    test_parse("(\\x.(x x))");
    test_parse( "((\\x.xx)(\\x.xy))");
}
