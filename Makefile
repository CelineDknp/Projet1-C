all: malloc

malloc.o: malloc.c
	@echo "Building malloc object"
	@gcc malloc.c -c -o malloc.o

malloc_test.o: malloc_test.c
	@echo "Building test object"
	@gcc malloc_test.c -c -o malloc_test.o -I/Users/Seb/local/include

tests: malloc.o malloc_test.o
	@echo "Creating test"
	@echo "Linking objects"
	@gcc -o tests malloc.o malloc_test.o -lcunit -L/Users/Seb/local/lib

malloc: malloc.o
	@echo "Creating malloc"
	@gcc -o malloc malloc.o

.PHONY: clean

clean:
	rm -rf *.o
