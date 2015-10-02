FLAGS := -Wall -O3 -pedantic -ggdb
GCC := gcc

sc: sc.c sc.h
	$(GCC) $(FLAGS) -o sc sc.c -lpthread

clean:
	rm -vf sc || true
