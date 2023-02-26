CFLAGS := -g -Wall -Werror -pedantic -fanalyzer \
		  -Iinclude 
GC_LIBS:=`pkg-config --libs bdw-gc`
CC=gcc

BUILD_DIR=./build
OBJ_DIR=./build
SRC_DIR=./src

HEADERS=$(wildcard include/*.h)
SRCS=src/lam.c # $(wildcard src/*.c)
OBJS=$(SRCS:src/%.c=build/%.o)

run-tests: ./build/tests
	$<

$(BUILD_DIR)/tests: tests.c $(OBJS)
	$(CC) $(CFLAGS) -Iutest.h  -o $@ $^ $(GC_LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) 
	$(CC) -c -o $@ $< $(CFLAGS) $(GC_LIBS)

clean:
	find ./build/ -type f -delete


