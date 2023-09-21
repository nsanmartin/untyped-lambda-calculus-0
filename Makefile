CFLAGS:=-g -Wall -Wextra -pedantic -Iinclude -Ibdwgc/include
STRICT_CFLAGS:= -Werror

# GC_LIBS:=$(shell pkg-config --libs bdw-gc)
# GC_LIBS:=bdwgc/extra/gc.c
GC=bdwgc/extra/gc.c

# CC=gcc -fanalyzer
# CC=zig cc
# CC=clang

BUILD_DIR=./build
OBJ_DIR=./build
SRC_DIR=./src
PARSER_DIR=./parser

HEADERS=$(wildcard include/*.h)
SRCS=$(wildcard src/*.c)
OBJS=$(SRCS:src/%.c=build/%.o)


run-tests: $(BUILD_DIR)/utests $(BUILD_DIR)/itests
	$(BUILD_DIR)/utests
	$(BUILD_DIR)/itests


$(BUILD_DIR)/utests: utests.c $(OBJS) $(BUILD_DIR)/gc.o
	$(CC) $(LAMF) $(STRICT_CFLAGS) $(CFLAGS) -Iutest.h  -o $@ $^ $(GC_LIBS)

$(BUILD_DIR)/itests: itests.c $(OBJS) $(BUILD_DIR)/gc.o $(PARSER_DIR)/lex.yy.c
	echo TODO: check -fanalyzer
	$(CC) -DLAM_LIB_PARSER $(LAMF) $(CFLAGS) \
		-Iutest.h -I$(PARSER_DIR) -o $@ $^ $(GC_LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) 
	$(CC) $(LAMF) $(STRICT_CFLAGS) -c -o $@  $< $(CFLAGS) $(GC_LIBS)

$(BUILD_DIR)/parser: $(PARSER_DIR)/parser.tab.c $(PARSER_DIR)/lex.yy.c \
	$(OBJS) 
	$(CC) $(CFLAGS) -I$(PARSER_DIR) -o $@ $^ -lfl $(GC_LIBS)


$(BUILD_DIR)/gc.o:
	$(CC) -c -o $(BUILD_DIR)/gc.o bdwgc/extra/gc.c -I bdwgc/include/

$(PARSER_DIR)/lex.yy.c: lexer.l \
	$(PARSER_DIR)/parser.tab.h $(PARSER_DIR)/parser.tab.c 
	flex -o $@ $<

$(PARSER_DIR)/parser.tab.h $(PARSER_DIR)/parser.tab.c: parser.y
	bison -t -d -o $@ $<

tags: $(HEADERS) $(SRCS) lexer.l parser.y utests.c itests.c
	universal-ctags -R .

clean:
	find ./build/ -type f -delete


