#ifndef __ZDLMALLOC_DEF_H__
#define __ZDLMALLOC_DEF_H__

//Base on dlmalloc 2.5
//
//
///* 
//SBRK_UNIT is a good power of two to call sbrk with It should
//normally be system page size or a multiple thereof.  If sbrk is very
//slow on a system, it pays to increase this.  Otherwise, it should
//not matter too much.
//*/
////zdf what is SBRK
//#define SBRK_UNIT 8192
//
//
///* 
//MAX_PREALLOCS is the maximum number of chunks to preallocate.  The
//actual number to prealloc depends on the available space in a
//selected victim, so typical numbers will be less than the maximum.
//Because of this, the exact value seems not to matter too much, at
//least within values from around 1 to 100.  Since preallocation is
//heuristic, making it too huge doesn't help though. It may blindly
//create a lot of chunks when it turns out not to need any more, and
//then consolidate them all back again immediatetly. While this is
//pretty fast, it is better to avoid it.
//*/
//#define MAX_PREALLOCS 5
//
//
//
//#ifndef __STD_C
//#	ifdef __STDC__
//#		define	__STD_C		1
//#	else
//#		if __cplusplus
//#			define __STD_C		1
//#		else
//#			define __STD_C		0
//#		endif /*__cplusplus*/
//#	endif /*__STDC__*/
//#endif /*__STD_C*/
//
//
//
//#ifndef _BEGIN_EXTERNS_
//#	if __cplusplus
//#		define _BEGIN_EXTERNS_	extern "C" {
//#		define _END_EXTERNS_	}
//#	else
//#		define _BEGIN_EXTERNS_
//#		define _END_EXTERNS_
//#	endif
//#endif /*_BEGIN_EXTERNS_*/
//
//
//
//#ifndef _ARG_
//#	if __STD_C
//#		define _ARG_(x)	x
//#	else
//#		define _ARG_(x)	()
//#	endif
//#endif /*_ARG_*/
//
//
//
//#ifndef Void_t
//#	if __STD_C
//#		define Void_t		void
//#	else
//#		define Void_t		char
//#	endif
//#endif /*Void_t*/
//
//
//
//#ifndef NIL
//#	define NIL(type)	((type)0)
//#endif /*NIL*/
//
//
//
//#if __STD_C
//#	include <stddef.h>   /* for size_t */
//#else
//#	include	<sys/types.h>
//#endif
//
//#include <stdio.h>    /* needed for malloc_stats */
//
//
//
//#ifdef __cplusplus
//extern "C" {
//#endif
//
//extern Void_t*     sbrk _ARG_((size_t));
//
///* mechanics for getpagesize; adapted from bsd/gnu getpagesize.h */
//
//#if defined(BSD) || defined(DGUX) || defined(sun) || defined(HAVE_GETPAGESIZE)
//	extern size_t getpagesize();
//#  define malloc_getpagesize getpagesize()
//#else
//#  include <sys/param.h>
//#  ifdef EXEC_PAGESIZE
//#    define malloc_getpagesize EXEC_PAGESIZE
//#  else
//#    ifdef NBPG
//#      ifndef CLSIZE
//#        define malloc_getpagesize NBPG
//#      else
//#        define malloc_getpagesize (NBPG * CLSIZE)
//#      endif
//#    else 
//#      ifdef NBPC
//#        define malloc_getpagesize NBPC
//#      else
//#        define malloc_getpagesize SBRK_UNIT    /* just guess */
//#      endif
//#    endif 
//#  endif
//#endif 
//
//#ifdef __cplusplus
//};  /* end of extern "C" */
//#endif





/*  CHUNKS */
struct malloc_chunk
{
	size_t size;				/* Size in bytes, including overhead. */
								/* Or'ed with INUSE if in use. */
	struct malloc_chunk* fd;	/* double links -- used only if free. */
	struct malloc_chunk* bk;
};
typedef struct malloc_chunk* mchunkptr;


/*  sizes, alignments */
#define SIZE_SZ              (sizeof(size_t))
#define MALLOC_MIN_OVERHEAD  (SIZE_SZ + SIZE_SZ)
#define MALLOC_ALIGN_MASK    (MALLOC_MIN_OVERHEAD - 1)
#define MINSIZE              (sizeof(struct malloc_chunk) + SIZE_SZ)



/* pad request bytes into a usable size */
#define request2size(req)	( ((long)(req) <= 0) ?  MINSIZE : (((req) + MALLOC_MIN_OVERHEAD + MALLOC_ALIGN_MASK) & ~(MALLOC_ALIGN_MASK)))

/* Check if m has acceptable alignment */
#define aligned_OK(m)		( ( (size_t)((m)) & (MALLOC_ALIGN_MASK) ) == 0 )

/* Check if a chunk is immediately usable */
#define exact_fit(ptr, req) ( (unsigned)((ptr)->size - (req)) < MINSIZE )

/* maintaining INUSE via size field */
#define INUSE  0x1		/* size field is or'd with INUSE when in use */
						/* INUSE must be exactly 1, so can coexist with size */


#define inuse(p)			( (p)->size & INUSE )
#define set_inuse(p)		( (p)->size |= INUSE )
#define clear_inuse(p)		( (p)->size &= ~INUSE )








#endif//__ZDLMALLOC_DEF_H__