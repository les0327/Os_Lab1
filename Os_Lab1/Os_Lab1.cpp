// OS_Lab1.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

using namespace std;

Header header1;
Header *header;

void test();

int main()
{
	allocator_init();
	test();
	allocator_free();

	char end;
	cin >> end;
	
	return 0;
}

void test() {
	mem_dump();

	int *arr = (int *) mem_alloc(100);
	arr[0] = 0;
	arr[1] = 1;
	arr[2] = 2;
	mem_dump();

	int *arr1 = (int *)mem_alloc(100);
	arr1[0] = 0;
	arr1[1] = 1;
	arr1[2] = 2;
	mem_dump();

	arr = (int*) mem_realloc(arr, 38);
	arr[0] = -1;
	//mem_dump();

	arr = (int*)mem_realloc(arr1, 200);
	//mem_dump();

	mem_free(arr1);
	mem_dump();

	mem_free(arr);
	mem_dump();
}