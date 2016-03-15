#include <CUnit.h>
#include "malloc.h"

void test_malloc_pointeur_non_null() {
	void* ptr = mymalloc(4);
	CU_ASSERT_PTR_NOT_NULL(ptr);
}

void test_malloc_deux_pointeurs_differents() {
	void* ptr = mymalloc(4);
	void* ptr2= mymalloc(4);
	CU_ASSERT_PTR_NOT_EQUAL(ptr, ptr2);
}

void test_malloc_fusion_bloc(){
	void* ptr1=mymalloc(16);
	void* ptr2=mymalloc(32);
	void* ptr3=mymalloc(64);
	free(ptr2);
	free(ptr1);
	void* ptr4=mymalloc(40);
	CU_ASSERT_PTR_EQUAL(ptr1, ptr4);
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
}
