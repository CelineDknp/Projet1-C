gcc -o malloc malloc.c

gcc -I$(HOME)/local/include -o tests malloc_test.c
