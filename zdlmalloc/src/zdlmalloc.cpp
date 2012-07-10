#include "zdlmalloc.h"





void free_returned_list()
{
	clear_last_remainder;
	while (returned_list != 0)
	{
		mchunkptr p = returned_list;
		returned_list = p->fd;
		clear_inuse(p);
		dirtylink(p);
	}
}



size_t gcd(size_t a, size_t b)
{
	size_t tmp;

	if (b > a)
	{
		tmp = a; a = b; b = tmp;
	}
	for(;;)
	{
		if (b == 0)
			return a;
		else if (b == 1)
			return b;
		else
		{
			tmp = b;
			b = a % b;
			a = tmp;
		}
	}
}


size_t  lcm(size_t x, size_t y)
{
  return x / gcd(x, y) * y;
}


mchunkptr malloc_from_sys(size_t nb)
{

  /* The end of memory returned from previous sbrk call */
  static size_t* last_sbrk_end = 0; 

  mchunkptr p;            /* Will hold a usable chunk */
  size_t*   ip;           /* to traverse sbrk ptr in size_t units */
  char*     cp;           /* result of sbrk call */
  
  /* Find a good size to ask sbrk for.  */
  /* Minimally, we need to pad with enough space */
  /* to place dummy size/use fields to ends if needed */

  size_t sbrk_size = ((nb + SBRK_UNIT - 1 + SIZE_SZ + SIZE_SZ) 
                       / SBRK_UNIT) * SBRK_UNIT;


  cp = (char*)(SBRK(sbrk_size));
  if (cp == (char*)(-1)) /* sbrk returns -1 on failure */
    return 0;

  ip = (size_t*)cp;

  sbrked_mem += sbrk_size;

  if (last_sbrk_end != &ip[-1]) /* Is this chunk continguous with last? */
  {                             
    /* It's either first time through or someone else called sbrk. */
    /* Arrange end-markers at front & back */

    /* Shouldn't be necessary, but better to be safe */
    while (!aligned_OK(ip)) { ++ip; sbrk_size -= SIZE_SZ; }

    /* Mark the front as in use to prevent merging. (End done below.) */
    /* Note we can get away with only 1 word, not MINSIZE overhead here */

    *ip++ = SIZE_SZ | INUSE;
    
    p = (mchunkptr)ip;
    set_size(p,sbrk_size - (SIZE_SZ + SIZE_SZ)); 
    
  }
  else 
  {
    mchunkptr l;  

    /* We can safely make the header start at end of prev sbrked chunk. */
    /* We will still have space left at the end from a previous call */
    /* to place the end marker, below */

    p = (mchunkptr)(last_sbrk_end);
    set_size(p, sbrk_size);

    /* Even better, maybe we can merge with last fragment: */

    l = prev_chunk(p);
    if (!inuse(l))  
    {
      unlink(l);
      set_size(l, p->size + l->size);
      p = l;
    }

  }

  /* mark the end of sbrked space as in use to prevent merging */

  last_sbrk_end = (size_t*)((char*)p + p->size);
  *last_sbrk_end = SIZE_SZ | INUSE;

  return p;
}


mchunkptr malloc_find_space(size_t nb)
{
  /* Circularly traverse bins so as not to pick on any one too much */
  static mbinptr rover = LASTBIN;    /* Circular roving ptr */

  mbinptr origin = rover;
  mbinptr b      = rover;

  /* Preliminaries.  */
  clear_last_remainder;
  reset_maxClean;

  do
  {
    mchunkptr p;

    while ( (p = last_dirty(b)) != dirty_head(b))
    {
      unlink(p);
      consolidate(p);

      if (p->size >= nb)
      {
        rover = b;
        return p;
      }
      else
        cleanlink(p);
    }

    b = (b == FIRSTBIN)? LASTBIN : b - 1;      /* circularly sweep down */

  } while (b != origin);

  /* If no return above, chain to the next step of malloc */
  return  malloc_from_sys(nb);
}


void malloc_clean_bin(mbinptr bin)
{
  mchunkptr p;

  clear_last_remainder;
  
  while ( (p = last_dirty(bin)) != dirty_head(bin))
  {
    unlink(p);
    consolidate(p);
    cleanlink(p);
  }

  while (returned_list != 0)
  {
    p = returned_list;
    returned_list = p->fd;
    clear_inuse(p);
    consolidate(p);
    cleanlink(p);
  }
}



Void_t* zdlmalloc(size_t bytes)
{
  static size_t previous_request = 0;  /* To control preallocation */

  size_t    nb = request2size(bytes);  /* padded request size */
  mbinptr   bin;                       /* corresponding bin */
  mchunkptr victim;                    /* will hold selected chunk */

  /* ----------- Peek at returned_list; hope for luck */

  if ((victim = returned_list) != 0 && 
      exact_fit(victim, nb)) /* size check works even though INUSE set */
  {
    returned_list = victim->fd;
    return chunk2mem(victim);
  }
  
  findbin(nb, bin);  /*  Need to know bin for other traversals */

  /* ---------- Scan dirty list of own bin */

     /* Code for small bins special-cased out since */
     /* no size check or traversal needed and */
     /* clean bins are exact matches so might as well test now */

  if (nb < MAX_SMALLBIN_SIZE)
  {
    if ( ((victim = first_dirty(bin)) != dirty_head(bin)) ||
         ((victim = last_clean(bin))  != clean_head(bin)))
    {
      unlink(victim);
      set_inuse(victim);
      return chunk2mem(victim);
    }
  }
  else
  {
    if ( (victim = first_dirty(bin)) != dirty_head(bin))
    {
      do
      {
        if (exact_fit(victim, nb))
        {
          unlink(victim);
          set_inuse(victim);
          return chunk2mem(victim);
        }
        else victim = victim->fd;
      } while (victim != dirty_head(bin));
      
      /* If there were chunks in there but none matched, then */
      /* consolidate all chunks in this bin plus those on free list */
      /* to prevent further traversals and fragmentation. */
      
      malloc_clean_bin(bin);
    }
  }
    
  /* ------------ Search free list */

  if ( (victim = returned_list) != 0)
  {
    do
    {
      mchunkptr next = victim->fd;
      if (exact_fit(victim, nb))
      {
        returned_list = next;
        return chunk2mem(victim);
      }
      else    /* Place unusable chunks in their bins  */
      {
        clear_inuse(victim);
        dirtylink(victim);
        victim = next;
      }
    } while (victim != 0);
    returned_list = 0;
  }

  /* -------------- Try the remainder from last successful split */

  if ( (victim = last_remainder) != 0 && victim->size >= nb)
  {
    last_remainder = 0; /* reset for next time */
    goto split;
  }

  /* -------------- Scan through own clean bin */

      /* (Traversing back-to-front tends to choose `old' */
      /*  chunks that could not be further consolidated.) */

  for (victim = last_clean(bin); victim != clean_head(bin); victim=victim->bk)
  {
    if (victim->size >= nb)
    {
      unlink(victim); 
      goto split;
    }
  }

  /* -------------- Try all bigger clean bins */

      /* (Scanning upwards is slower but prevents fragmenting big */
      /* chunks that we might need later. If there aren't any smaller */
      /* ones, most likely we got a big one from last_remainder anyway.) */

  {
    mbinptr b;

    for (b = bin + 1; b <= maxClean; ++b)
    {
      if ( (victim = last_clean(b)) != clean_head(b) ) 
      {
        unlink(victim);
        goto split;
      }
    }
  }

  /* -------------  Consolidate or sbrk */

  victim =  malloc_find_space(nb);

  if (victim == 0)  /* propagate failure */
    return 0; 

  /* -------------- Possibly split victim chunk */

 split:  
  {
    size_t room = victim->size - nb;
    if (room >= MINSIZE)     
    {
      mchunkptr v = victim;  /* Hold so can break up in prealloc */
      
      set_size(victim, nb);  /* Adjust size of chunk to be returned */
      
      /* ---------- Preallocate */
      
          /* Try to preallocate some more of this size if */
          /* last (split) req was of same size */
      
      if (previous_request == nb)
      {
        int i;
        
        for (i = 0; i < MAX_PREALLOCS && room >= nb + MINSIZE; ++i)
        {
          room -= nb;
           
          v = (mchunkptr)((char*)(v) + nb); 
          set_size(v, nb);
          set_inuse(v);     /* free-list chunks must have inuse set */
          return_chunk(v);  /* add to free list */
        } 
      }

      previous_request = nb;  /* record for next time */

      /* ---------- Create remainder chunk  */
      
      /* Get rid of the old one first */
      if (last_remainder != 0) cleanlink(last_remainder);
      
      last_remainder = (mchunkptr)((char*)(v) + nb);
      set_size(last_remainder, room);
    }

    set_inuse(victim);
    return chunk2mem(victim);
  }
}




void zdlfree(Void_t* mem)
{
  if (mem != 0)
  {
    mchunkptr p = mem2chunk(mem);
    return_chunk(p);
  }
}

Void_t* zdlrealloc(Void_t* mem, size_t bytes)
{
  if (mem == 0) 
    return zdlmalloc(bytes);
  else
  {
    size_t       nb      = request2size(bytes);
    mchunkptr    p       = mem2chunk(mem);
    size_t       oldsize;
    long         room;
    mchunkptr    nxt;

    if (p == returned_list) /* support realloc-last-freed-chunk idiocy */
       returned_list = returned_list->fd;

    clear_inuse(p);
    oldsize = p->size;

    /* try to expand (even if already big enough), to clean up chunk */

    free_returned_list(); /* make freed chunks available to consolidate */

    while (!inuse(nxt = next_chunk(p))) /* Expand the chunk forward */
    {
      unlink(nxt);
      set_size(p, p->size + nxt->size);
    }

    room = p->size - nb;
    if (room >= 0)          /* Successful expansion */
    {
      if (room >= MINSIZE)  /* give some back if possible */
      {
        mchunkptr remainder = (mchunkptr)((char*)(p) + nb);
        set_size(remainder, room);
        cleanlink(remainder);
        set_size(p, nb);
      }
      set_inuse(p);
      return chunk2mem(p);
    }
    else /* Could not expand. Get another chunk and copy. */
    {
      Void_t* newmem;
      size_t count;
      size_t* src;
      size_t* dst;

      set_inuse(p);    /* don't let malloc consolidate us yet! */
      newmem = zdlmalloc(nb);

      if (newmem != 0) {
        /* Copy -- we know that alignment is at least `size_t' */
        src = (size_t*) mem;
        dst = (size_t*) newmem;
        count = (oldsize - SIZE_SZ) / sizeof(size_t);
        while (count-- > 0) *dst++ = *src++;
      }

      zdlfree(mem);
      return newmem;
    }
  }
}


Void_t* memalign(size_t alignment, size_t bytes)
{
  mchunkptr p;
  size_t    nb = request2size(bytes);
  size_t    room;

  /* find an alignment that both we and the user can live with: */
  /* least common multiple guarantees mutual happiness */
  size_t    align = lcm(alignment, MALLOC_MIN_OVERHEAD);

  /* call malloc with worst case padding to hit alignment; */
  /* we will give back extra */

  size_t req = nb + align + MINSIZE;
  Void_t*  m = zdlmalloc(req);

  if (m == 0) return 0; /* propagate failure */

  p = mem2chunk(m);
  clear_inuse(p);


  if (((size_t)(m) % align) != 0) /* misaligned */
  {

    /* find an aligned spot inside chunk */

    mchunkptr ap = (mchunkptr)((((size_t)(m) + align-1) & -align) - SIZE_SZ);

    size_t gap = (size_t)(ap) - (size_t)(p);

    /* we need to give back leading space in a chunk of at least MINSIZE */

    if (gap < MINSIZE)
    {
      /* This works since align >= MINSIZE */
      /* and we've malloc'd enough total room */

      ap = (mchunkptr)( (size_t)(ap) + align );
      gap += align;    
    }

    room = p->size - gap;

    /* give back leader */
    set_size(p, gap);
    dirtylink(p); /* Don't really know if clean or dirty; be safe */

    /* use the rest */
    p = ap;
    set_size(p, room);
  }

  /* also give back spare room at the end */

  room = p->size - nb;
  if (room >= MINSIZE)
  {
    mchunkptr remainder = (mchunkptr)((char*)(p) + nb);
    set_size(remainder, room);
    dirtylink(remainder); /* Don't really know; be safe */
    set_size(p, nb);
  }

  set_inuse(p);
  return chunk2mem(p);

}



Void_t* valloc(size_t bytes)
{
  /* Cache result of getpagesize */
  static size_t malloc_pagesize = 0;

  if (malloc_pagesize == 0) malloc_pagesize = malloc_getpagesize;
  return memalign (malloc_pagesize, bytes);
}


Void_t* zdlcalloc(size_t n, size_t elem_size)
{
  size_t sz = n * elem_size;
  Void_t* p = zdlmalloc(sz);
  char* q = (char*) p;
  while (sz-- > 0) *q++ = 0;
  return p;
}


void cfree(Void_t *mem)
{
  zdlfree(mem);
}


size_t malloc_usable_size(Void_t* mem)
{
  if (mem == 0)
    return 0;
  else
  {
    mchunkptr p = (mchunkptr)((char*)(mem) - SIZE_SZ); 
    size_t sz = p->size & ~(INUSE);
    /* report zero if not in use or detectably corrupt */
    if (p->size == sz || sz != *((size_t*)((char*)(p) + sz - SIZE_SZ)))
      return 0;
    else
      return sz - MALLOC_MIN_OVERHEAD;
  }
}

void malloc_stats()
{

  /* Traverse through and count all sizes of all chunks */

  size_t avail = 0;
  size_t malloced_mem;

  mbinptr b;

  free_returned_list();

  for (b = FIRSTBIN; b <= LASTBIN; ++b)
  {
    mchunkptr p;

    for (p = first_dirty(b); p != dirty_head(b); p = p->fd)
      avail += p->size;

    for (p = first_clean(b); p != clean_head(b); p = p->fd)
      avail += p->size;
  }

  malloced_mem = sbrked_mem - avail;

  fprintf(stderr, "total mem = %10u\n", sbrked_mem);
  fprintf(stderr, "in use    = %10u\n", malloced_mem);

}








int main(int argn, const char* argv[])
{
	int i = 0;
	i = i * 10;
}