#include "parser.tab.c"

#define test_parse(S)                                                   \
    printf("parsing '%s' => ", S);                                      \
    parse_string( S "\n");

int main () {
    test_parse("x");
    test_parse("(x x)");
    test_parse("(\\x.(x x))");
    test_parse("(\\x.(x x))");
    test_parse( "((\\x.xx)(\\x.xy))");
}
