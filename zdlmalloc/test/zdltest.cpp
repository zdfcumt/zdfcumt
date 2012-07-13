// zdltest.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#include "zdlmalloc.h"
#include "malloc-2.8.4.h"
#include <assert.h>

void test_zdl();
void test_malloc_284();

int _tmain(int argc, _TCHAR* argv[])
{
	//test_zdl();
	test_malloc_284();
	return 0;
}

void test_zdl()
{
	void* pMemArr[10240];
	for (int j = 0; j < 5;j++)
	{
		for (int i = 3; i < 10240; i+=16) {
			pMemArr[i] = valloc(i);
		}
		for (int i = 3; i < 10240; i+=16) {
			zdlfree(pMemArr[i]);
			pMemArr[i] = NULL;
		}
	}
}



void test_malloc_284()
{

}
