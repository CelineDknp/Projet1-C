#include <CUnit.h>
##include "malloc.h"

void test_malloc_pointeur_non_null() {
	void* ptr = mymalloc(4);
	CU_ASSERT_PTR_NOT_NULL(ptr);
}

void test_malloc_deux_pointeurs_differents() {
	void* ptr = mymalloc(4);
	void* ptr2= mymalloc(4);
	CU_ASSERT_PTR_NOT_EQUAL(ptr, ptr2);
}

void test_free_block_reallouable() {
	void* ptr = mymalloc(4);
	myfree(ptr);
	void* ptr2= mymalloc(4);
	CU_ASSERT_PTR_EQUAL(ptr, ptr2);
}
