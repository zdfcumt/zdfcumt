#ifndef __ZDLMALLOC_H__
#define __ZDLMALLOC_H__

#include "zdlmalloc_def.h"



/* A helper for realloc */
static void free_returned_list();			//zdf done


/* Utilities needed below for memalign */
/* Standard greatest common divisor algorithm */
static size_t gcd(size_t a, size_t b);

static size_t  lcm(size_t x, size_t y);


/* Dealing with sbrk */
/* This is one step of malloc; broken out for simplicity */
static size_t sbrked_mem = 0; /* Keep track of total mem for malloc_stats */
static mchunkptr malloc_from_sys(size_t nb);	//zdf done



/* Consolidate dirty chunks until create one big enough for current req. */
/* Call malloc_from_sys if can't create one. */
/* This is just one phase of malloc, but broken out for sanity */
static mchunkptr malloc_find_space(size_t nb);	//zdf done


/* Clear out dirty chunks from a bin, along with the free list. */
/* Invoked from malloc when things look too fragmented */
static void malloc_clean_bin(mbinptr bin);		//zdf done


/*   Finally, the user-level functions  */
Void_t* zdlmalloc(size_t bytes);		//zdf done



void zdlfree(Void_t* mem);				//zdf done


Void_t* zdlrealloc(Void_t* mem, size_t bytes);		//zdf done



/* Return a pointer to space with at least the alignment requested */
/* Alignment argument should be a power of two */
Void_t* memalign(size_t alignment, size_t bytes);	//zdf done


/* Derivatives */
Void_t* valloc(size_t bytes);						//zdf done


Void_t* zdlcalloc(size_t n, size_t elem_size);		//zdf done


void cfree(Void_t *mem);							//zdf done



size_t malloc_usable_size(Void_t* mem);



void malloc_stats();




#endif//__ZDLMALLOC_H__