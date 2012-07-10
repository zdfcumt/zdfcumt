#ifndef __ZDLMALLOC_DEF_H__
#define __ZDLMALLOC_DEF_H__

//Base on dlmalloc 2.5

#ifndef NOMACRO
#define NOMACRO 1
#endif
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




#ifndef _BEGIN_EXTERNS_
#	if __cplusplus
#		define _BEGIN_EXTERNS_	(extern "C" {)
#		define _END_EXTERNS_	(})
#	else
#		define _BEGIN_EXTERNS_
#		define _END_EXTERNS_
#	endif
#endif /*_BEGIN_EXTERNS_*/



#define _ARG_(x)	x


#ifndef Void_t
#	define Void_t		void
#endif /*Void_t*/



#ifndef NIL
#	define NIL(type)	((type)0)
#endif /*NIL*/



#include <stddef.h>   /* for size_t */
#include <stdio.h>    /* needed for malloc_stats */
#ifdef WIN32
#include <windows.h>
#else

#endif


#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#define SBRK(SIZE)		((Void_t*)VirtualAlloc(NULL, (SIZE), MEM_COMMIT, PAGE_READWRITE))
#else
extern Void_t*     sbrk _ARG_((size_t));
#define SBRK(SIZE)		(sbrk(SIZE))
#endif



//* mechanics for getpagesize; adapted from bsd/gnu getpagesize.h */
#ifdef WIN32
/* getpagesize for windows */
static long getpagesize (void) {
	static long g_pagesize = 0;
	if (! g_pagesize) {
		SYSTEM_INFO system_info;
		GetSystemInfo (&system_info);
		g_pagesize = system_info.dwPageSize;
	}
	return g_pagesize;
}
#define malloc_getpagesize getpagesize()
#else
extern size_t getpagesize();
#define malloc_getpagesize getpagesize()
#endif

#ifdef __cplusplus
};  /* end of extern "C" */
#endif




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
#define SIZE_SZ              (sizeof(size_t))								//4
#define MALLOC_MIN_OVERHEAD  (SIZE_SZ + SIZE_SZ)							//8
#define MALLOC_ALIGN_MASK    (MALLOC_MIN_OVERHEAD - 1)						//7
#define MINSIZE              (sizeof(struct malloc_chunk) + SIZE_SZ)		//16



/* pad request bytes into a usable size */
//zdf 返回一个以MALLOC_MIN_OVERHEAD为步进的至少比给定参数大MALLOC_MIN_OVERHEAD的返回值
#if NOMACRO
static size_t request2size(size_t req)
{
	if (req <= 0){
		return MINSIZE;
	}else{
		return (req + MALLOC_MIN_OVERHEAD + MALLOC_ALIGN_MASK) & ~(MALLOC_ALIGN_MASK);
	}
}
#else
#define request2size(req)	( ((long)(req) <= 0) ?  MINSIZE : (((req) + MALLOC_MIN_OVERHEAD + MALLOC_ALIGN_MASK) & ~(MALLOC_ALIGN_MASK)))
#endif

/* Check if m has acceptable alignment */
#define aligned_OK(m)		( ( (size_t)((m)) & (MALLOC_ALIGN_MASK) ) == 0 )

/* Check if a chunk is immediately usable */
#if NOMACRO
static bool exact_fit(mchunkptr ptr, size_t req){
	return ((ptr->size - req) < MINSIZE);
}
#else
#define exact_fit(ptr, req) ( (unsigned)((ptr)->size - (req)) < MINSIZE )
#endif

/* maintaining INUSE via size field */
#define INUSE  0x1		/* size field is or'd with INUSE when in use */
						/* INUSE must be exactly 1, so can coexist with size */


#define inuse(p)			( (p)->size & INUSE )
#define set_inuse(p)		( (p)->size |= INUSE )
#define clear_inuse(p)		( (p)->size &= ~INUSE )



/* Physical chunk operations  */
/* Ptr to next physical malloc_chunk. */
#define next_chunk(p)		((mchunkptr)( ((char*)(p)) + ((p)->size & ~INUSE) ))

/* Ptr to previous physical malloc_chunk */
#define prev_chunk(p)		((mchunkptr)( ((char*)(p)) - ( *((size_t*)((char*)(p) - SIZE_SZ)) & ~INUSE)))

/* place size at front and back of chunk */
#define set_size(P, Sz)														  \
{ 																			  \
  size_t Sss = (Sz);														  \
  (P)->size = *((size_t*)((char*)(P) + Sss - SIZE_SZ)) = Sss;				  \
}																			  \


/* conversion from malloc headers to user pointers, and back */
#define chunk2mem(p)		( (Void_t*)((char*)(p) + SIZE_SZ) )
#define mem2chunk(mem)		( (mchunkptr)((char*)(mem) - SIZE_SZ) )



/* BINS */
struct malloc_bin
{
  struct malloc_chunk dhd;		/* dirty list header */
  struct malloc_chunk chd;		/* clean list header */
};

typedef struct malloc_bin* mbinptr;


/* field-extraction macros */
#define clean_head(b)		(&((b)->chd))
#define first_clean(b)		((b)->chd.fd)
#define last_clean(b)		((b)->chd.bk)

#define dirty_head(b)		(&((b)->dhd))
#define first_dirty(b)		((b)->dhd.fd)
#define last_dirty(b)		((b)->dhd.bk)



/* The bins, initialized to have null double linked lists */
#define		NBINS		128             /* for 32 bit addresses */
#define		LASTBIN		(&(av[NBINS-1]))
#define		FIRSTBIN	(&(av[2]))      /* 1st 2 bins unused but simplify indexing */


/* Bins < MAX_SMALLBIN_OFFSET are special-cased since they are 8 bytes apart */
#define		MAX_SMALLBIN_OFFSET		18
#define		MAX_SMALLBIN_SIZE		144  /* Max size for which small bin rules apply */


/* Helper macro to initialize bins */
#define IAV(i)	{{ 0, &(av[i].dhd),  &(av[i].dhd) }, { 0, &(av[i].chd),  &(av[i].chd) }}



static struct malloc_bin  av[NBINS] = 
{
  IAV(0),   IAV(1),   IAV(2),   IAV(3),   IAV(4), 
  IAV(5),   IAV(6),   IAV(7),   IAV(8),   IAV(9),
  IAV(10),  IAV(11),  IAV(12),  IAV(13),  IAV(14), 
  IAV(15),  IAV(16),  IAV(17),  IAV(18),  IAV(19),
  IAV(20),  IAV(21),  IAV(22),  IAV(23),  IAV(24), 
  IAV(25),  IAV(26),  IAV(27),  IAV(28),  IAV(29),
  IAV(30),  IAV(31),  IAV(32),  IAV(33),  IAV(34), 
  IAV(35),  IAV(36),  IAV(37),  IAV(38),  IAV(39),
  IAV(40),  IAV(41),  IAV(42),  IAV(43),  IAV(44), 
  IAV(45),  IAV(46),  IAV(47),  IAV(48),  IAV(49),
  IAV(50),  IAV(51),  IAV(52),  IAV(53),  IAV(54), 
  IAV(55),  IAV(56),  IAV(57),  IAV(58),  IAV(59),
  IAV(60),  IAV(61),  IAV(62),  IAV(63),  IAV(64), 
  IAV(65),  IAV(66),  IAV(67),  IAV(68),  IAV(69),
  IAV(70),  IAV(71),  IAV(72),  IAV(73),  IAV(74), 
  IAV(75),  IAV(76),  IAV(77),  IAV(78),  IAV(79),
  IAV(80),  IAV(81),  IAV(82),  IAV(83),  IAV(84), 
  IAV(85),  IAV(86),  IAV(87),  IAV(88),  IAV(89),
  IAV(90),  IAV(91),  IAV(92),  IAV(93),  IAV(94), 
  IAV(95),  IAV(96),  IAV(97),  IAV(98),  IAV(99),
  IAV(100), IAV(101), IAV(102), IAV(103), IAV(104), 
  IAV(105), IAV(106), IAV(107), IAV(108), IAV(109),
  IAV(110), IAV(111), IAV(112), IAV(113), IAV(114), 
  IAV(115), IAV(116), IAV(117), IAV(118), IAV(119),
  IAV(120), IAV(121), IAV(122), IAV(123), IAV(124), 
  IAV(125), IAV(126), IAV(127)
};


/* 
  Auxiliary lists 
  Even though they use bk/fd ptrs, neither of these are doubly linked!
  They are null-terminated since only the first is ever accessed.
  returned_list is just singly linked for speed in free().
  last_remainder currently has length of at most one.
*/
static	mchunkptr	returned_list = 0;		/* List of (unbinned) returned chunks */
static	mchunkptr	last_remainder = 0;		/* last remaindered chunk from malloc */



/* 
  Indexing into bins 
  Funny-looking mechanics: 
    For small bins, the index is just size/8.
    For others, first find index corresponding to the power of 2
        closest to size, using a variant of standard base-2 log
        calculation that starts with the first non-small index and
        adjusts the size so that zero corresponds with it. On each
        iteration, the index is incremented across the four quadrants
        per power of two. (This loop runs a max of 27 iterations;
        usually much less.) After the loop, the remainder is quartered
        to find quadrant. The offsets for loop termination and
        quartering allow bins to start, not end at powers.
*/
#if NOMACRO
static void findbin(size_t& Sizefb, mbinptr& Bfb)
{
	size_t Sfb = (Sizefb);
	if (Sfb < MAX_SMALLBIN_SIZE)
		(Bfb) = (av + (Sfb >> 3));
	else
	{
		/* Offset wrt small bins */
		size_t Ifb = MAX_SMALLBIN_OFFSET;
		Sfb >>= 3;
		/* find power of 2 */
		while (Sfb >= (MINSIZE * 2)) { Ifb += 4; Sfb >>= 1; }
		/* adjust for quadrant */
		Ifb += (Sfb - MINSIZE) >> 2;
		(Bfb) = av + Ifb;
	}
}

#else
#define findbin(Sizefb, Bfb)												  \
{																			  \
  size_t Sfb = (Sizefb);													  \
  if (Sfb < MAX_SMALLBIN_SIZE)												  \
    (Bfb) = (av + (Sfb >> 3));												  \
  else																		  \
  {																			  \
    /* Offset wrt small bins */												  \
    size_t Ifb = MAX_SMALLBIN_OFFSET;										  \
    Sfb >>= 3;																  \
	/* find power of 2 */													  \
    while (Sfb >= (MINSIZE * 2)) { Ifb += 4; Sfb >>= 1; }					  \
	/* adjust for quadrant */												  \
    Ifb += (Sfb - MINSIZE) >> 2;                            				  \
    (Bfb) = av + Ifb;														  \
  }																			  \
}																			  \


#endif


/* Keep track of the maximum actually used clean bin, to make loops faster */
/* (Not worth it to do the same for dirty ones) */
static mbinptr maxClean = FIRSTBIN;
#if NOMACRO
static void reset_maxClean_f(){
	while (maxClean > FIRSTBIN && clean_head(maxClean)==last_clean(maxClean))
	--maxClean;
}
#define	reset_maxClean	reset_maxClean_f()

#else

#define reset_maxClean														  \
{																			  \
  while (maxClean > FIRSTBIN && clean_head(maxClean)==last_clean(maxClean))	  \
    --maxClean;																  \
}																			  \

#endif




/* Macros for linking and unlinking chunks */
/* take a chunk off a list */
#define unlink(Qul)															  \
{																			  \
  mchunkptr Bul = (Qul)->bk;												  \
  mchunkptr Ful = (Qul)->fd;												  \
  Ful->bk = Bul;  Bul->fd = Ful;											  \
}																			  \



/* place a chunk on the dirty list of its bin */
#define dirtylink(Qdl)														  \
{																			  \
  mchunkptr Pdl = (Qdl);													  \
  mbinptr   Bndl; 															  \
  mchunkptr Hdl, Fdl;														  \
																			  \
  findbin(Pdl->size, Bndl);													  \
  Hdl  = dirty_head(Bndl); 													  \
  Fdl  = Hdl->fd; 															  \
																			  \
  Pdl->bk = Hdl;  Pdl->fd = Fdl;  Fdl->bk = Hdl->fd = Pdl;					  \
}																			  \


/* Place a consolidated chunk on a clean list */
#define cleanlink(Qcl)														  \
{																			  \
  mchunkptr Pcl = (Qcl);													  \
  mbinptr Bcl; 																  \
  mchunkptr Hcl, Fcl;														  \
																			  \
  findbin(Qcl->size, Bcl);													  \
  Hcl  = clean_head(Bcl); 													  \
  Fcl  = Hcl->fd; 															  \
  if (Hcl == Fcl && Bcl > maxClean) maxClean = Bcl;							  \
																			  \
  Pcl->bk = Hcl;  Pcl->fd = Fcl;  Fcl->bk = Hcl->fd = Pcl;					  \
}																			  \




/* consolidate one chunk */
#define consolidate(Qc)														  \
{																			  \
  for (;;)																	  \
  {																			  \
    mchunkptr Pc = prev_chunk(Qc);											  \
    if (!inuse(Pc))															  \
    {																		  \
      unlink(Pc);															  \
      set_size(Pc, Pc->size + (Qc)->size);									  \
      (Qc) = Pc;															  \
    }																		  \
    else break;																  \
  }																			  \
  for (;;)																	  \
  {																			  \
    mchunkptr Nc = next_chunk(Qc);											  \
    if (!inuse(Nc))															  \
    {																		  \
      unlink(Nc);															  \
      set_size((Qc), (Qc)->size + Nc->size);								  \
    }																		  \
    else break;																  \
  }																			  \
}																			  \




/* Place the held remainder in its bin */
/* This MUST be invoked prior to ANY consolidation */
#if NOMACRO
static void clear_last_remainder_f(){
	if (last_remainder != 0)
	{
		cleanlink(last_remainder);
		last_remainder = 0;
	}
};
#define clear_last_remainder (clear_last_remainder_f())
#else

#define clear_last_remainder												  \
{																			  \
  if (last_remainder != 0)													  \
  {																			  \
    cleanlink(last_remainder);												  \
    last_remainder = 0;														  \
  }																			  \
}																			  \

#endif

/* Place a freed chunk on the returned_list */
#define return_chunk(Prc)													  \
{																			  \
  (Prc)->fd = returned_list;												  \
  returned_list = (Prc); 													  \
}																			  \




#endif//__ZDLMALLOC_DEF_H__