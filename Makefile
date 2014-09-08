fitness: alloc.c args.c main.c
	gcc -o Test -std=c99 -g -Og -save-temps alloc.c main.c args.c
