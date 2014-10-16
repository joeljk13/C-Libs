
SHELL = /bin/sh
SRCS = ./src/alloc.c ./src/main.c
SRCOBJS = $(SRCS:.c=.o)
TESTS = ./tests/alloc.c 
TESTOBJS = $(TESTS:c=.o)
CPARAMS = -std=c99  -pedantic-errors -g -Og -DDEBUG
LPARAMS = 
LINKS = 
test: $(SRCOBJS)
	gcc -o $@ $(LPARAMS) $^ $(LINKS)
src/alloc.o: src/alloc.c src/main.h src/alloc.h
	gcc -c -o $@ $(CPARAMS) $<
src/main.o: src/main.c src/main.h src/alloc.h
	gcc -c -o $@ $(CPARAMS) $<

.PHONY: check
check: $(TESTOBJS) ;
tests/alloc.o: tests/alloc.c tests/../src/main.h tests/../src/alloc.h \
 tests/../src/main.h
	gcc -c -o $@ $(CPARAMS) $<         
	gcc -o check $(LPARAMS) $@ ./src/alloc.o $(LINKS)         
	if ! ./check; then echo 'check failed'; fi         
	rm check $@
.PHONY: clean
clean:
	rm -rf test ./src/stack.o ./src/alloc.o ./src/main.o 
