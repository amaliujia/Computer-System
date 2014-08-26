/* mm.c
 *
 * Algorithm: 
 * 	 This malloc mainly uses segregated list and first hit.
 *
 * Segregated lists:
 *	 Free blocks are divided into 8 groups by a power of 2. More specifically, 
 *	 the first one is greater than 32 bytes(2^5), and is less than 64(bytes), 
 *	 and so on. Head pointers of 8 list are saved in the beginning of heap in 
 *	 order.
 *
 * Manipulating free lists:
 * 	 When malloc is called, it tries to scan free lists. Uaually, it starts 
 * 	 from list a by first hit, where a counts from 0 and 2^(5+a) <= requested size <=
 * 	 2^(6+a). If it does't find any available block in a, it will search 
 * 	 next unitl gets one or stops by end. If no free block is pick up, malloc
 * 	 asks for a new chunk space for heap. 
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "contracts.h"

#include "mm.h"
#include "memlib.h"


#define ALIGNMENT   8
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))
#define SIZE_PTR(p) ((size_t *)p)
#define SIZE_MOVE(p)    (((p + 1) >> 1) << 1)
#define SIZE_DMOVE(p)   (p & MP)
#define SIZE_PDMOVE(p)	((*(size_t *)p) & MN)
#define MP	0x0000000000000001
#define MN	0xfffffffffffffffe
// Create aliases for driver tests
// DO NOT CHANGE THE FOLLOWING!
#ifdef DRIVER
#define malloc mm_malloc
#define free mm_free
#define realloc mm_realloc
#define calloc mm_calloc
#endif

/*
 *  Logging Functions
 *  -----------------
 *  - dbg_printf acts like printf, but will not be run in a release build.
 *  - checkheap acts like mm_checkheap, but prints the line it failed on and
 *    exits if it fails.
 */

#ifndef NDEBUG
#define dbg_printf(...) printf(__VA_ARGS__)
#define checkheap(verbose) do {if (mm_checkheap(verbose)) {  \
                             printf("Checkheap failed on line %d\n", __LINE__);\
                             exit(-1);  \
                        }}while(0)
#else
#define dbg_printf(...)
#define checkheap(...)
#endif

#define SIZE1			(1 << 6)
#define SIZE2			(1 << 7)
#define SIZE3			(1 << 8)
#define SIZE4			(1 << 9)
#define SIZE5			(1 << 10)
#define	SIZE6			(1 << 11)
#define SIZE7			(1 << 12)
#define SIZE8			(1 << 13) 
#define SIZE9           (1 << 14)
#define SIZE10          (1 << 15)
#define SIZE11          (1 << 16)
#define SIZE12          (1 << 17)
#define SIZE13          (1 << 18)
#define SIZE14          (1 << 19)
#define SIZE15          (1 << 20)                         
#define SIZEOFLIST		16
#define HEADOFFSET		(8 * SIZEOFLIST) 
/*
 *  Helper functions
 *  ----------------
 */

// Align p to a multiple of w bytes
static inline void* align(const void const* p, unsigned char w) {
    return (void*)(((uintptr_t)(p) + (w-1)) & ~(w-1));
}

// Check if the given pointer is 8-byte aligned
static inline int aligned(const void const* p) {
    return align(p, 8) == p;
}

// Return whether the pointer is in the heap.
static int in_heap(const void* p) {
    return p <= mem_heap_hi() && p >= mem_heap_lo();
}

// my funciton declaration
static inline void listadd(unsigned char *p);

/*
 *  Block Functions
 *  ---------------
 *  TODO: Add your comment describing block functions here.
 *  The functions below act similar to the macros in the book, but calculate
 *  size in multiples of 4 bytes.
 */

// Return the size of the given block in multiples of the word size
static inline unsigned int block_size(const uint32_t* block) {
    REQUIRES(block != NULL);
    REQUIRES(in_heap(block));

    return (block[0] & 0x3FFFFFFF);
}

// Return true if the block is free, false otherwise
static inline int block_free(const uint32_t* block) {
    REQUIRES(block != NULL);
    REQUIRES(in_heap(block));

    return !(block[0] & 0x40000000);
}

// Mark the given block as free(1)/alloced(0) by marking the header and footer.
static inline void block_mark(uint32_t* block, int free) {
    REQUIRES(block != NULL);
    REQUIRES(in_heap(block));

    unsigned int next = block_size(block) + 1;
    block[0] = free ? block[0] & (int) 0xBFFFFFFF : block[0] | 0x40000000;
    block[next] = block[0];
}

// Return a pointer to the memory malloc should return
static inline uint32_t* block_mem(uint32_t* const block) {
    REQUIRES(block != NULL);
    REQUIRES(in_heap(block));
    REQUIRES(aligned(block + 1));

    return block + 1;
}

// Return the header to the previous block
static inline uint32_t* block_prev(uint32_t* const block) {
    REQUIRES(block != NULL);
    REQUIRES(in_heap(block));

    return block - block_size(block - 1) - 2;
}

// Return the header to the next block
static inline uint32_t* block_next(uint32_t* const block) {
    REQUIRES(block != NULL);
    REQUIRES(in_heap(block));

    return block + block_size(block) + 2;
}

/*
 *
 *	private funciton
 *
 */
int blocksize(const unsigned char *p){
    REQUIRES(p != NULL);
    REQUIRES(in_heap(p));

	return *(size_t *)p & ~1;
}

int blockmask(const unsigned char *p){
    REQUIRES(p != NULL);
    REQUIRES(in_heap(p));

	return *(size_t *)p & 1;	
} 

unsigned char *blocknext(const unsigned char *p){
    REQUIRES(p != NULL);
    REQUIRES(in_heap(p));
	REQUIRES(in_heap(p + blocksize(p)));

	return (unsigned char *)(p + blocksize(p));
}

unsigned char *blockprev(const unsigned char *p){
    REQUIRES(p != NULL);
    REQUIRES(in_heap(p));
	REQUIRES(in_heap(p - 8));
	REQUIRES((p - 8) >= ((unsigned char *)mem_heap_lo() + HEADOFFSET));

	return (unsigned char *)(p - ((*(size_t *)(p - 8)) & ~1));
}

/*
 * getsizeoffset mapps size into offset, by which we get located
 * proper free block list.   		
 */
static inline int getsizeoffset(size_t size){
	if(size < 32){ 
		printf("allocated wrong size,  size %zu\n",  size);
		exit(1);
	}else if(size <= SIZE1){
		return 0;
	}else if(size <= SIZE2){
		return 1;
	}else if(size <= SIZE3){
		return 2;
	}else if(size <= SIZE4){
		return 3;
	}else if(size <= SIZE5){
		return 4;
	}else if(size <= SIZE6){
		return 5;
	}else if(size <= SIZE7){
		return 6;
	}else if(size <= SIZE8){
        return 7;
    }else if(size <= SIZE9){
        return 8;
    }else if(size <= SIZE10){
        return 9;
    }else if(size <= SIZE11){
        return 10;
    }else if(size <= SIZE12){
        return 11;
    }else if(size <= SIZE13){
        return 12;
    }else if(size <= SIZE14){
        return 13;
    }else if(size <= SIZE15){
        return 14;
    }else{
        return 15;
    }
}

// nalloc extends heap when free block cannot statisfy requests.
static inline void *nalloc(size_t size){
	   	size_t newsize = ALIGN(size + 16);
		if(newsize <= 24){
			newsize = ALIGN(size + 24);
		}
        unsigned char *p;
		p = mem_sbrk(newsize);
        if((long)p < 0) return NULL;
        else{
            *(size_t *)p = (newsize | 1);
            *(size_t *)(p + newsize - 8) = (newsize | 1);
	     	return p + 8;       
        }
}

// this fucntion split large chunk blick into smalller pieces.
static inline void* split(unsigned char *p, size_t oldsize, size_t newsize){
	if((oldsize - newsize) < 32){
		*(size_t *)p = oldsize | 1;
        *(size_t *)(p + oldsize - 8) = oldsize | 1;
	}else{
                *(size_t *)(p + newsize) = (oldsize - newsize);
                *(size_t *)(p + oldsize - 8) = oldsize - newsize;
                *(size_t *)p = newsize | 1;
				*(size_t *)(p + newsize - 8) = newsize | 1;
				listadd(p + newsize);
	}
	return p + 8;
}

// this function remove free block from list.
static inline void listdelete( unsigned char *block){
	size_t size = *(size_t *)block & MN;
	int offset = getsizeoffset(size);
	unsigned char *p = (unsigned char *)mem_heap_lo() + offset * 8;	
	//block is first one
	if(*(size_t *)p == (size_t)block){
		*(size_t *)p = *(size_t *)(block + 8);
		if(*(size_t *)(block + 8) != 0)	*(size_t *)(*(size_t *)(block + 8) + 16) = 0;
		return;
	}	
	//block is the middle	
	if((*(size_t *)(block + 8) != 0) && (*(size_t *)(block + 16) != 0)){
            *(size_t *)(*(size_t *)(block + 16) + 8) = (*(size_t *)(block + 8));
            *(size_t *)(*(size_t *)(block + 8) + 16) = *(size_t *)(block + 16);
			return;
	}	
	//block is the last one
    *(size_t *)(*(size_t *)(block + 16) + 8) = 0;
}	

//add free block into list
static inline void listadd(unsigned char *p){
	int offset = getsizeoffset(*(size_t *)p);
    unsigned char *head = mem_heap_lo();
	unsigned char *target = (head + offset * 8);	
	if(*(size_t *)target == 0){
		*(size_t *)target = (size_t)p;
		*(size_t *)(p + 8) = 0;
		*(size_t *)(p + 16) = 0;
	}else{
		*(size_t *)(((unsigned char *)(*(size_t *)target)) + 16) = (size_t)p;
	    *(size_t *)(p + 8) = *(size_t *)target;
		*(size_t *)(p + 16) = 0;
		*(size_t *)target = (size_t)p;
	}
	checkheap(1);
}

// find available block from block lists
static inline void* findhit(size_t size){
	unsigned char *head = mem_heap_lo();
	int offset = getsizeoffset(size);
	unsigned char *p = head + offset * 8;
	size_t newsize = size;	
	head += (SIZEOFLIST * 8);
	while((p < head) && (*(size_t *)p == 0)){
		p += 8;
	}
	if(p >= head)	return NULL;
	unsigned char *cursor = (unsigned char *)*(size_t *)p;	
	//start to search a good one
	while(p < head){
		while((cursor != 0) &&(*(size_t *)cursor < newsize)){
			cursor = (unsigned char *)*(size_t *)(cursor + 8);
		}
		if(cursor != 0){
			break;
		}else{
			p += 8;
			cursor = (unsigned char *)*(size_t *)p;
		}
	}
	if(p >= head)	return NULL;
	size_t oldsize = *(size_t *)cursor;	
	//let's search
	listdelete(cursor);

	// splite big free block into small chunks
	return split(cursor, oldsize, newsize);					
}


// coalesec free blocks in the neighbor.
static inline void coalesce(void *para){
	unsigned char *p = (unsigned char *)para;
	unsigned char *head = mem_heap_lo();
	unsigned char *next = NULL;
	unsigned char *prev = NULL;
	int left = 0, right = 0;
	if(in_heap(p + *(size_t *)p) && ((*(size_t *)(p + *(size_t *)p) & 1) == 0)){
		right = 1;
        next = p + *(size_t *)p;
	}
	
	if(in_heap(p - 8) && ((p - 8) >= (head + SIZEOFLIST * 8)) && 
								((*(size_t *)(p - 8) & 1) == 0)){
		left = 1;
        prev = p - *(size_t *)(p - 8);
	}
	// if left block and right block are not free	
	if(!left && !right){
		p = p;	
	}else if(left && !right){//if left neighbor is free
		listdelete(prev);
		size_t ps = *(size_t *)(p - 8);	
		size_t cs = *(size_t *)p;
		*(size_t *)prev += cs;
		*(size_t *)(p + cs - 8) += ps;
		p = prev;	
	}else if(!left && right){// if right is free
		listdelete(next);
		size_t cs = *(size_t *)p;
		size_t ns = *(size_t *)next;
		*(size_t *)p += ns;
		*(size_t *)(next + ns - 8) += cs;
		p = p;
	}else{ // if both are free
		listdelete(next);
        listdelete(prev);
		size_t ps = *(size_t *)(p - 8);
		size_t cs = *(size_t *)p;
		size_t ns = *(size_t *)next;
		*(size_t *)prev += (cs + ns);
		*(size_t *)(next + ns - 8) += (cs + ps);	
		p = prev;
	}
		listadd(p);
}
/*
 *  Malloc Implementation
 *  ---------------------
 *  The following functions deal with the user-facing malloc implementation.
 */

/*
 * Initialize: return -1 on error, 0 on success.
 */
int mm_init(void){ 
	unsigned char *p;
	int i;
	p = mem_sbrk(8 * SIZEOFLIST);
	if((long)p < 0){
		printf("Initialization failed\n");
		return -1;
	}
	for(i = 0; i < SIZEOFLIST; i++)
		*(size_t *)(p + i * 8) = 0;
	return 0;
}

/*
 * malloc
 */
void *malloc (size_t size) {
	assert(size);
    checkheap(1);  // Let's make sure the heap is ok!
	size_t newsize = ALIGN(size + 16);
	if(newsize <= 24){
		newsize = ALIGN(size + 24);
	}
	unsigned char *p, *head;
	head = mem_heap_lo();
	
	p = findhit(newsize);
	if(p != NULL){
		return p;
	}
    return nalloc(size);	
}

/*
 * free
 */
void free (void *ptr) {
   if (ptr == NULL) {
        return;
    }
	unsigned char *p = (unsigned char *)ptr - 8;
	size_t size = (*(size_t *)p) & MN;

	*(size_t *)p = size;
	*(size_t *)(p + size - 8) = size;
	
	coalesce(p);
}

/*
 * realloc - you may want to look at mm-naive.c
 */
void *realloc(void *oldptr, size_t size) {
  size_t oldsize;
  void *newptr;
  
  /* If size == 0 then this is just free, and we return NULL. */
  if(size == 0) {
    free(oldptr);
    return 0;
  }

  /* If oldptr is NULL, then this is just malloc. */
  if(oldptr == NULL) {
    return malloc(size);
  }

  newptr = malloc(size);

  /* If realloc() fails the original block is left untouched  */
  if(!newptr) {
    return 0;
  }

  /* Copy the old data. */
  oldsize = ((*(size_t *)((unsigned char *)oldptr - 8)) & MN) - 16;
;
  if(size < oldsize) oldsize = size;
  memcpy(newptr, oldptr, oldsize);

  /* Free the old block. */
  free(oldptr);

  return newptr;
}

/*
 * calloc - you may want to look at mm-naive.c
 */
void *calloc (size_t nmemb, size_t size) {
  size_t bytes = nmemb * size;
  void *newptr;

  newptr = malloc(bytes);
  memset(newptr, 0, bytes);

  return newptr;
}

// Returns 0 if no errors were found, otherwise returns the error
int mm_checkheap(int verbose) {
    verbose = verbose;
	unsigned char *p = (unsigned char *)mem_heap_lo() + 64;   
	unsigned char *upperbound = (unsigned char *)mem_heap_lo() + SIZEOFLIST * 8;
	unsigned char *next = NULL; 
	size_t size;

	while(in_heap(p)){
		// check alignment
		size = *(size_t *)p;
		*(size_t *)p = size & ~1;
		if(!aligned(p)){
			printf("addr %zu, size %zu do not align\n", (size_t)p, *(size_t *)p);
			exit(1);
		}
		//check header and footer
		*(size_t *)p = size;	
		size = size & ~1;	
		if(*(size_t *)p != *(size_t *)(p + size - 8)){
            printf("addr %p, head %zu, foot %zu do not align\n", 
            			p, *(size_t *)p, *(size_t *)(p + *(size_t *)p - 8));
            exit(1);
		}
		//check consecution 
		if(in_heap(p + size)){
			next = p + size;
			if(((*(size_t *)p & 1) == 0) && ((*(size_t *)next & 1) == 0)){
				printf("addr %p, and addr %p are consecutive\n", p, next);
			}
			// check block overlapping
			if((size + p) > next){
				printf("oops, overlap happens: %p : %p overlap %p : %p\n", 
					p, p + size - 1, next, next + (*(size_t *)next & ~1) - 1);
				exit(1);
			} 
		}
		p += size;
	}

	//check free list
	unsigned char *t =(unsigned char *)mem_heap_lo();
	unsigned char *head;
	for(t = (unsigned char *)mem_heap_lo(); t < upperbound; t += 8){	
		head = (unsigned  char *)(*(size_t *)t);
		//if free block is in heap	
		while(head != 0){
			if(!in_heap(head)){
				printf("free block %p are not in heap, heap size %zu and t = %d\n",
							head, mem_heapsize(), 
							(int)(t-(unsigned char *)mem_heap_lo()) / 8);
				exit(1);
			}
			//check consistency
			if((*(size_t *)(head + 8) != 0) && in_heap((unsigned char *)(*(size_t *)(head + 8)))){
				next = (unsigned char *)(*(size_t *)(head + 8));
				if((unsigned char *)*(size_t *)(next + 16) != head){
					printf("free block %p and %p are inconsistent\n", head, next);
					exit(1);
				} 	
			}else if(*(size_t *)(head + 8) == 0){
				break;
			}else{
				printf("free block are no int heap, wrong pointer %p and pointing to %p, size: %zu\n", 
									p, 
									(unsigned char *)(*(size_t *)(head + 8)), 
									(*(size_t *)p));
				exit(1);
			}
		head = (unsigned char *)(*(size_t *)(head + 8));	
		}
	}

	// check list size and count free blocks
	head = (unsigned char *)mem_heap_lo();
	int i, count = 0;
	for(head = (unsigned char *)mem_heap_lo(), i = 0; head < upperbound; head += 8, i += 1){
		p = (unsigned char *)(*(size_t *)head);
		while(p != 0){
			if(getsizeoffset(*(size_t *)p) != i){
				printf("block %p, size %zu is in wrong list\n", p, *(size_t *)p);
			}
			count++;
			p = (unsigned char *)(*(size_t *)(p + 8));	
		}	
	}	

	head = upperbound;
	while(in_heap(head)){
		if((*(size_t *)head & 1) == 0){
				count--;
		}
		head += (*(size_t *)head & ~1);
	}

	if(count != 0){
		printf("momery leek %d\n", count);
		exit(1);
	}
	return 0;
}
