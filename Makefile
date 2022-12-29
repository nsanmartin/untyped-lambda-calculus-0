CC=gcc

run-tests: tests
	./build/tests

tests:
	$(CC) -g -I./include -I./utest.h -o ./build/$@ ./$@.c ./src/lam.c

clean:
	rm ./build/*

