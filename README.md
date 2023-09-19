Deps:

Tests:

Using github.com/sheredom/utest.h as a submodule

```
git clone --recursive git://github.com/nsanmartin/untyped-lambda-calculus.git
```

or `git submodule update --init --recursive` if already cloned.

When added gc valgrind detected errors, so a flag asenabled for a
simple memory managment only for testing purposes to confirm or
not whether they are false positive. To use it write:


```
make LAMF="-DTESTMEM"
```
