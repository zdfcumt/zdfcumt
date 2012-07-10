// zdltest.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "zdlmalloc.h"
#include <assert.h>

int _tmain(int argc, _TCHAR* argv[])
{
	//for (int i = 1; i < 1024; ++i) {
		void* pMem = zdlmalloc(17);
		assert(NULL != pMem);
		zdlfree(pMem);
		pMem = NULL;
	//}
	return 0;
}

