// zdltest.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "zdlmalloc.h"
#include <assert.h>

int _tmain(int argc, _TCHAR* argv[])
{
	void* pMemArr[1024];
	for (int i = 3; i < 1024; i+=8) {
		pMemArr[i] = zdlmalloc(102410);
	}
	for (int i = 3; i < 1024; i+=8) {
		zdlfree(pMemArr[i]);
		pMemArr[i] = NULL;
	}
	return 0;
}

