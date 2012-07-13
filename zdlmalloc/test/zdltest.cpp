// zdltest.cpp : 定义控制台应用程序的入口点。
//

#define ZDLMALLOC	0
#define MALLOC284	1

#include "stdafx.h"
#if ZDLMALLOC
#include "zdlmalloc.h"
#endif
#if MALLOC284
#include "malloc-2.8.4.h"
#endif


#include <assert.h>

#if ZDLMALLOC
void test_zdl();
#endif

#if MALLOC284
void test_malloc_284();
#endif


int _tmain(int argc, _TCHAR* argv[])
{
#if ZDLMALLOC
	test_zdl();
#endif

#if MALLOC284
	test_malloc_284();
#endif

	return 0;
}

#if ZDLMALLOC
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
#endif


#if MALLOC284
void test_malloc_284()
{
	void* pMem = dlmalloc(244);
	dlfree(pMem);
	pMem = dlmalloc(245);
	dlfree(pMem);
}
#endif
