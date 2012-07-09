#ifndef __ZDLMALLOC_DEF_H__
#define __ZDLMALLOC_DEF_H__

//Base on dlmalloc 2.5


/* 
SBRK_UNIT is a good power of two to call sbrk with It should
normally be system page size or a multiple thereof.  If sbrk is very
slow on a system, it pays to increase this.  Otherwise, it should
not matter too much.
*/
//zdf what is SBRK
#define SBRK_UNIT 8192


/* 
MAX_PREALLOCS is the maximum number of chunks to preallocate.  The
actual number to prealloc depends on the available space in a
selected victim, so typical numbers will be less than the maximum.
Because of this, the exact value seems not to matter too much, at
least within values from around 1 to 100.  Since preallocation is
heuristic, making it too huge doesn't help though. It may blindly
create a lot of chunks when it turns out not to need any more, and
then consolidate them all back again immediatetly. While this is
pretty fast, it is better to avoid it.
*/
#define MAX_PREALLOCS 5



#ifndef __STD_C
	#ifdef __STDC__
		#define	__STD_C		1
	#else
		#if __cplusplus
			#define __STD_C		1
		#else
			#define __STD_C		0
		#endif /*__cplusplus*/
	#endif /*__STDC__*/
#endif /*__STD_C*/



#ifndef _BEGIN_EXTERNS_
	#if __cplusplus
		#define _BEGIN_EXTERNS_	extern "C" {
		#define _END_EXTERNS_	}
	#else
		#define _BEGIN_EXTERNS_
		#define _END_EXTERNS_
	#endif
#endif /*_BEGIN_EXTERNS_*/











#endif//__ZDLMALLOC_DEF_H__