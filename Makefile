FLAGS := -Wall -O3 -pedantic
GCC := gcc

client: client.c client.h lib.h lib.o
	$(GCC) $(FLAGS) -o client lib.o client.c

lib.o: lib.h lib.c
	$(GCC) $(FLAGS) -c lib.c

clean:
	rm -vf *.o client server || true
