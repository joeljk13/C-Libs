
SHELL = /bin/sh
SRCS = alloc.c main.c
OBJS = $(SRCS:.c=.o)
test: $(OBJS)
	gcc -o $@  $^ 
alloc.o: alloc.c main.h alloc.h
	gcc -c -o $@ -std=c99 -O2  -pedantic-errors -g -Og -DDEBUG $<
main.o: main.c main.h alloc.h
	gcc -c -o $@ -std=c99 -O2  -pedantic-errors -g -Og -DDEBUG $<

.PHONY: clean
clean:
	rm -f *.o test
