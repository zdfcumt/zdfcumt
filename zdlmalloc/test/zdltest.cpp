// zdltest.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "zdlmalloc.h"
#include <assert.h>

void test_zdl();

int _tmain(int argc, _TCHAR* argv[])
{
	test_zdl();

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




