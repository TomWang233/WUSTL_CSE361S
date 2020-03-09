/* Followed the discussion in 9.9.14 of 
 * CSAPP 3e. Used the idea of Segregated 
 * Free List method and the First Fit strategy.
 * Thus, we can create 17 size classes:
 * (1 - 2^4),(2^4+1 - 2^5),(2^5+1 - 2^6)...(2^18+1 - 2^19),(2^19+1 - INF).
 * The allocator maintains an array of free lists on the
 * heap.
 * Each contains free blocks of sizes within the certain range.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "NoTeamName",
    /* First member's full name */
    "Chenyang Wang",
    /* First member's email address */
    "chenyang.wang@wustl.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define INVALID_PTR ((void*) -1)  /* Invalid pointer */
#define ALLOC 1                   /* Alloc tag */
#define FREE  0                   /* Free tag */

#define WSIZE 4                   /* Word and header/footer size (bytes) */
#define DSIZE 8                   /* Double word size (bytes) */
#define CHUNKSIZE (1<<12)         /* Extend heap by this amount (bytes) */

#define SIZE_CLASS_NUM 17         /* Number of size classes */
#define MIN_BLOCK_SIZE (4*WSIZE)  /* Minimum block size */

#define MAX(x,y) ((x)>(y)?(x):(y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p) (*(unsigned int*) (p))
#define PUT(p, val) (*(unsigned int*) (p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1) 

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char*) (bp) - WSIZE)
#define FTRP(bp) ((char*) (bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define PREV_BLKP(bp) ((char*)(bp) - GET_SIZE(((char*)(bp) - DSIZE)))
#define NEXT_BLKP(bp) ((char*)(bp) + GET_SIZE(HDRP(bp)))

/* Given block ptr bp, compute address of its predecessor and successor */
#define PRED(bp) ((char*) (bp))
#define SUCC(bp) ((char*) (bp) + WSIZE)

/* Given block ptr bp, compute address of predecessor and successor blocks */
#define PRED_BLKP(bp) ((char*) GET(PRED(bp)))
#define SUCC_BLKP(bp) ((char*) GET(SUCC(bp)))

static char*  heap_head = NULL; /*first block pointer of the heap*/
static char** list_start = NULL; /*pointer to the start of the free list*/

/*helper functions*/
static void* extend_heap(int words);
static void* coalesce(void* bp);
static void  place(void* bp, size_t mysize);
static int   size_class_get(size_t mysize);
static bool  checklist(void* p);
static void  deletelist(void* bp);
static void  insertlist(void* bp);
static void* findlist(size_t mysize);

/* 
 * extend_heap - Extend the heap by a given number of words.
 */
static void* extend_heap(int words){
	int num = (words % 2) ? (words + 1) : words;
    int size = num * WSIZE;
	char* bp = mem_sbrk(size);
    if (bp == INVALID_PTR) {
        return NULL;
    }
	PUT(HDRP(bp), PACK(size, 0));         /* header */
    PUT(FTRP(bp), PACK(size, 0));         /* footer */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* new epilogue */
	bp = coalesce(bp);
    insertlist(bp);
	return bp;
}

/* 
 * coalesce - Coalesce a free block as described in the book
 */
static void* coalesce(void* bp){
    int prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    int next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    int size = GET_SIZE(HDRP(bp));
	 if(prev_alloc==ALLOC&&next_alloc==ALLOC){     /*Case 1*/
		 return bp;
	 }
	 else if(prev_alloc==ALLOC&&next_alloc==FREE){ /*Case 2*/
		deletelist(NEXT_BLKP(bp));
		size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
	 }
	 else if(prev_alloc==FREE&&next_alloc==ALLOC){ /*Case 3*/
		deletelist(PREV_BLKP(bp));
		size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
	 }
	 else{                                         /*Case 4*/
		deletelist(PREV_BLKP(bp));
        deletelist(NEXT_BLKP(bp));
		size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
	 }
	 return bp;
 }
 
/* 
 * place - place a block of a size at bp, split if necessary
 */
static void place(void* bp, size_t mysize){
    size_t temp = GET_SIZE(HDRP(bp));
	/* need to split */
    if ((temp - mysize) >= MIN_BLOCK_SIZE) {
        deletelist(bp);
		/* allocate block */
        PUT(HDRP(bp), PACK(mysize, 1));
        PUT(FTRP(bp), PACK(mysize, 1));
		/* create new free block from the remainder */
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(temp - mysize, 0));
        PUT(FTRP(bp), PACK(temp - mysize, 0));
		/* insert new block into free list */
        PUT(PRED(bp), 0);
        PUT(SUCC(bp), 0);
        insertlist(bp);
    }
    /* no split */
    else {
        deletelist(bp);
        PUT(HDRP(bp), PACK(temp, 1));
        PUT(FTRP(bp), PACK(temp, 1));
    }
}
 
/* 
 * checklist - check whether a pointer points to free list head.
 */
static bool checklist(void* p){
    unsigned int val   = (unsigned int) p;
    unsigned int start = (unsigned int) list_start;
    unsigned int end   = start + WSIZE * (SIZE_CLASS_NUM - 1);
	if ((val < start) || (val > end) ||
        ((end - val) % WSIZE != 0)) {
        return false;
    }
	return true;
}
 
/* 
 * deletelist - delete a block from the free list.
 */
static void deletelist(void* bp){
    bool pre_tag = checklist(PRED_BLKP(bp));
    bool suc_tag = (SUCC_BLKP(bp) != NULL);
	if (GET_ALLOC(HDRP(bp)) == ALLOC) {
        return;
    }
	/* bp is the first block of a free list and has successors */
    if (pre_tag && suc_tag) {
        PUT(PRED_BLKP(bp), (unsigned int) SUCC_BLKP(bp));
        PUT(PRED(SUCC_BLKP(bp)), (unsigned int) PRED_BLKP(bp));
    }
    /* bp is both the first and the last block of a list */
    else if (pre_tag && !suc_tag) {
        PUT(PRED_BLKP(bp), (unsigned int) SUCC_BLKP(bp));
    }
    /* bp is an intermediate block */
    else if (!pre_tag && suc_tag) {
        PUT(SUCC(PRED_BLKP(bp)), (unsigned int) SUCC_BLKP(bp));
        PUT(PRED(SUCC_BLKP(bp)), (unsigned int) PRED_BLKP(bp));
    }
    /* bp is the last block */
    else {
        PUT(SUCC(PRED_BLKP(bp)), 0);
    }
	PUT(PRED(bp), 0);
    PUT(SUCC(bp), 0);
}
 
/* 
 * size_class_get - get size class based on the size.
 */
 static int size_class_get(size_t mysize){
    int size_class = 0;
    int remainder_sum = 0;
	while (mysize > MIN_BLOCK_SIZE && size_class < (SIZE_CLASS_NUM - 1)) {
        size_class++;
        remainder_sum += mysize % 2;
        mysize /= 2;
    }
	if (size_class < (SIZE_CLASS_NUM - 1) && remainder_sum > 0 && mysize == MIN_BLOCK_SIZE) {
        size_class++;
    }
	return size_class;
}
 
/* 
 * insertlist - insert a free block into the list.
 */
static void insertlist(void* bp){
    size_t size = GET_SIZE(HDRP(bp));   /* adjusted size */
    char** size_class;                  /* the first free block of the size class */
    unsigned int bp_val = (unsigned int) bp;
	/* get appropriate size class */
    size_class = list_start + size_class_get(size);
    /* the appropriate size class is empty */
    if (GET(size_class) == 0) {
        /* change heap array */
        PUT(size_class, bp_val);
        /* set pred/succ of new free block */
        PUT(PRED(bp), (unsigned int) size_class);
        PUT(SUCC(bp), 0);
    }
    /* the appropriate size class is not empty */
    else {
        /* insert the free block at the beginning of the size class */
        /* set pred/succ of new free block */
        PUT(PRED(bp), (unsigned int) size_class);
        PUT(SUCC(bp), GET(size_class));
        /* connect the previous head of the size class */
        PUT(PRED(GET(size_class)), bp_val);
        /* change heap array */
        PUT(size_class, bp_val);
    }
}

/* 
 * findlist - find free block fit according to first-fit policy.
 */
 static void* findlist(size_t mysize){
    int size_class = size_class_get(mysize);
    void* class;
    void* bp;
    size_t blk_size;
	while (size_class < SIZE_CLASS_NUM) {
        class = list_start + size_class;
        size_class++;
		if (GET(class) == 0) {
            continue;
        }
		bp = (void*) GET(class);
		/* traverse the list to find one */
        while (bp != NULL) {
            blk_size = GET_SIZE(HDRP(bp));
			/* found */
            if (mysize <= blk_size) {
                return bp;
            }
			bp = SUCC_BLKP(bp);
        }
    }
	/* not found */
    return NULL;
}
 
/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void){
    int num = SIZE_CLASS_NUM + 2 + 1;
    heap_head = mem_sbrk(WSIZE * num);
    if (heap_head == INVALID_PTR) {
        return -1;
    }
    memset(heap_head, 0, WSIZE * num);
    list_start = (char**) heap_head;
    heap_head += SIZE_CLASS_NUM * WSIZE;
    PUT(heap_head, PACK(DSIZE, 1));               /* prologue header */
    PUT(heap_head + (1 * WSIZE), PACK(DSIZE, 1)); /* prologue footer */
    PUT(heap_head + (2 * WSIZE), PACK(0, 1));     /* epilogue header */
    heap_head += (1 * WSIZE);  /* set heap_head pointer to prologue block */
    if (extend_heap(CHUNKSIZE / WSIZE) == NULL) {
        return -1;
    }
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void* mm_malloc(size_t size){
    if (size == 0) {
        return NULL;
    }
	size_t mysize;
    if (size <= DSIZE) {
        mysize = 2 * DSIZE;
    } else {
        mysize = DSIZE * ((size + DSIZE + (DSIZE - 1)) / DSIZE);
    }
	char* bp = findlist(mysize);
    /* find a block from free list */
    if (bp != NULL) {
        place(bp, mysize);
        return bp;
    }
	/* no fit block, extend heap */
    size_t esize = MAX(mysize, CHUNKSIZE);
    bp = extend_heap(esize / WSIZE);
    if (bp == NULL) {
        return NULL;
    }
    place(bp, mysize);
	return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void* ptr){	
	size_t size = GET_SIZE(HDRP(ptr));
	PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));
    PUT(PRED(ptr), 0);
    PUT(SUCC(ptr), 0);
    insertlist(coalesce(ptr));
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void* mm_realloc(void* ptr, size_t size)
{
    if (ptr == NULL) {
        return mm_malloc(size);
    }
	if (size == 0) {
        mm_free(ptr);
        return NULL;
    }
	size_t old_size = GET_SIZE(HDRP(ptr));
	/* adjust size */
    size_t mysize;
    if (size <= DSIZE) {
        mysize = 2 * DSIZE;
    } else {
        mysize = DSIZE * ((size + DSIZE + (DSIZE - 1)) / DSIZE);
    }
	/* same size */
    if (mysize == old_size) {
        return ptr;
    }
	void* old_ptr = ptr;
    /* growing */
    if (mysize > old_size) {
        /*
         * check whether can make the block large enough by
         * coalescing  */
		size_t next_blk_size = GET_SIZE(HDRP(NEXT_BLKP(ptr)));
        if (GET_ALLOC(HDRP(NEXT_BLKP(ptr))) == FREE) {
            if (next_blk_size + old_size >= mysize) {
                /* coalescing */
                deletelist(NEXT_BLKP(ptr));
				PUT(HDRP(ptr), PACK(old_size + next_blk_size, 1));
                PUT(FTRP(ptr), PACK(old_size + next_blk_size, 1));
				return old_ptr;
            }
        }
		/* free the current block and allocate
         * a new block and copy everything over*/
        void* new_ptr = mm_malloc(size);
        size_t copy_size = old_size - DSIZE;
        memcpy(new_ptr, old_ptr, copy_size);
        mm_free(ptr);
		return new_ptr;
    }
    /* shrinking */
    else {
        /* split the block */
        if (old_size - mysize >= MIN_BLOCK_SIZE) {
            /* shrink the old block */
            PUT(HDRP(ptr), PACK(mysize, 1));
            PUT(FTRP(ptr), PACK(mysize, 1));
			/* make a new block */
            void* new_ptr = NEXT_BLKP(ptr);
            PUT(HDRP(new_ptr), PACK(old_size - mysize, 0));
            PUT(FTRP(new_ptr), PACK(old_size - mysize, 0));
			PUT(PRED(new_ptr), 0);
            PUT(SUCC(new_ptr), 0);
			insertlist(new_ptr);
        }
		return old_ptr;
    }
}