// zdltest.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#include "zdlmalloc.h"
#include <assert.h>

int _tmain(int argc, _TCHAR* argv[])
{
	void* pMem = zdlmalloc(1024);
	assert(NULL != pMem);
	zdlfree(pMem);
	pMem = NULL;
	return 0;
}

