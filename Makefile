CFLAGS := -g -Wall -Werror -pedantic \
		  -Iinclude -Iutest.h -Idatatype99 -Imetalang99/include

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
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/tests-coverage: tests.c $(OBJS)
	$(CC) -fprofile-arcs -ftest-coverage -O0 $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) 
	$(CC) $(MKFLG) -c -o $@ $< $(CFLAGS) 

clean:
	find ./build/ -type f -delete


