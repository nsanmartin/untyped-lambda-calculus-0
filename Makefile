CFLAGS:=-g -Wall -Wextra -pedantic -Iinclude 
STRICT_CFLAGS:=-fanalyzer -Werror

GC_LIBS:=`pkg-config --libs bdw-gc`
CC=gcc

BUILD_DIR=./build
OBJ_DIR=./build
SRC_DIR=./src

HEADERS=$(wildcard include/*.h)
SRCS=$(wildcard src/*.c)
OBJS=$(SRCS:src/%.c=build/%.o)

run-tests: ./build/tests
	$<

$(BUILD_DIR)/tests: tests.c $(OBJS)
	$(CC) $(LAMF) $(STRICT_CFLAGS) $(CFLAGS) -Iutest.h  -o $@ $^ $(GC_LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) 
	$(CC) $(LAMF) $(STRICT_CFLAGS) -c -o $@ $< $(CFLAGS) $(GC_LIBS)

tags: $(HEADERS) $(SRCS) tests.c utest.h/utest.h
	universal-ctags $^ 


build/parser: build/parser.tab.c build/lex.yy.c $(OBJS) 
	$(CC) $(CFLAGS) -Ibuild -o $@ $^ -lfl $(GC_LIBS)

build/lex.yy.c: lexer.l build/parser.tab.h build/parser.tab.c 
	flex -o $@ $<

build/parser.tab.h build/parser.tab.c: parser.y
	bison -t -d -o $@ $<

clean:
	find ./build/ -type f -delete


