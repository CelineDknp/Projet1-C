#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <CUnit/CUnit.h>
#include "malloc.h"

extern size_t memsize;
extern size_t memloc;

void test_malloc_pointeur_non_null() {
	void* ptr = mymalloc(4);
	CU_ASSERT_PTR_NOT_NULL(ptr);
	myfree(ptr);
}

void test_malloc_ajout() {
	char* ptr = mymalloc(sizeof(char));
	*ptr='a';
	CU_ASSERT_EQUAL(*ptr, 'a');
	myfree(ptr);
}

void callocTest(void) {
	int *ptr = (int *) mycalloc(sizeof(int));
	CU_ASSERT_EQUAL(*ptr, 0);
	myfree(ptr);
}

void test_malloc_deux_pointeurs_differents() {
	void* ptr = mymalloc(4);
	void* ptr2= mymalloc(4);
	CU_ASSERT_PTR_NOT_EQUAL(ptr, ptr2);
	myfree(ptr);
	myfree(ptr2);
}

void test_malloc_size_0() {
	void* ptr = mymalloc(0);
	CU_ASSERT_PTR_NULL(ptr);
}

void test_free_block_reallouable() {
	void* ptr = mymalloc(4);
	myfree(ptr);
	void* ptr2= mymalloc(4);
	CU_ASSERT_PTR_EQUAL(ptr, ptr2);
	myfree(ptr2);
}

void test_free_malloc() {
	void* ptr = malloc(4);
	myfree(ptr);
	CU_ASSERT_PTR_NOT_NULL(ptr);
	free(ptr);
}

void test_double_free() {
	void* ptr = mymalloc(4);
	myfree(ptr);
	myfree(ptr);
	CU_ASSERT_PTR_NOT_NULL(ptr);
}

int setup(void) {
	memsize = 100;
	memloc = memsize;
	return 0;
}

int teardown(void) {
	return 0;
}

int main(int argc, char const *argv[]) {
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();
	CU_pSuite pSuite = NULL;
	pSuite = CU_add_suite("Suite de tests : malloc", setup, teardown);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL == CU_add_test(pSuite, "Blocks réallouables", test_free_block_reallouable)) ||
	 		(NULL == CU_add_test(pSuite, "Pointeur non null", test_malloc_pointeur_non_null)) ||
			(NULL == CU_add_test(pSuite, "Deux pointeurs différents", test_malloc_deux_pointeurs_differents)) ||
			(NULL == CU_add_test(pSuite, "Size 0", test_malloc_size_0)) ||
			(NULL == CU_add_test(pSuite, "Pointeur invalide", test_free_malloc)) ||
			(NULL == CU_add_test(pSuite, "Test valeur", test_malloc_ajout)) ||
			(NULL == CU_add_test(pSuite, "Double Free", test_double_free)) ||
			(NULL == CU_add_test(pSuite, "Test Calloc", callocTest))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	CU_basic_run_tests();
	CU_basic_show_failures(CU_get_failure_list());
	CU_cleanup_registry();
	return 0;
}
