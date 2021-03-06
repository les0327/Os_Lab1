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
	mem_dump("Start");

	int *arr = (int *) mem_alloc(100);
	arr[0] = 0;
	arr[1] = 1;
	arr[2] = 2;
	mem_dump("malloc 100");

	int *arr1 = (int *)mem_alloc(50);
	arr1[0] = 0;
	arr1[1] = 1;
	arr1[2] = 2;
	mem_dump("malloc 50");

	arr = (int*) mem_realloc(arr, 38);
	mem_dump("realloc 100 to 38");

	arr1 = (int*)mem_realloc(arr1, 200);
	mem_dump("realloc 50 to 200");

	mem_free(arr1);
	mem_dump("free 200");

	mem_free(arr);
	mem_dump("free 38");
}