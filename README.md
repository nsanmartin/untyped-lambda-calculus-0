Tests:

Using github.com/sheredom/utest.h as a submodule

When added gc valgrind detected errors, so a flag asenabled for a
simple memory managment only for testing purposes to confirm or
not whether they are false positive. To use it write:


```
make LAMF="-DTESTMEM"
```
