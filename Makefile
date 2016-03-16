all: malloc

malloc.o: malloc.c
	@echo "Building malloc object"
	@gcc malloc.c -c -o malloc.o

malloc_test.o: malloc_test.c
	@echo "Building test object"
	@gcc malloc_test.c -c -o malloc_test.o -I/Users/Seb/local/include

main.o: main.c
	@echo "Building main object"
	@gcc main.c -c -o main.o

tests: malloc.o malloc_test.o
	@echo "Creating test"
	@echo "Linking objects"
	@gcc -o tests malloc.o malloc_test.o -lcunit -L/Users/Seb/local/lib

malloc: malloc.o main.o
	@echo "Creating malloc"
	@gcc -o malloc main.o malloc.o

.PHONY: clean rm

clean:
	@echo "Removing object files"
	@rm -rf *.o

rm: clean
	@echo "Removing execs files"
	@rm -rf malloc tests malloc_printf
