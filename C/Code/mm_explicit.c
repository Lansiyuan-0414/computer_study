#define _DEFAULT_SOURCE
#define EXPLICIT 1
/* AI-assisted educational allocator. Linux, single-threaded, not libc replacement.
 * malloc/realloc sizes mean USER payload bytes, like the standard interface.
 * Build: gcc -std=c11 -Wall -Wextra -Werror -g -DDEBUG file.c -o demo
 */
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <limits.h>

#define CHUNKSIZE 4096u
#define MAX_HEAP (5u * 1024u * 1024u)
#if EXPLICIT
typedef uint64_t word_t;
#define WSIZE 8u
#define ALIGNMENT 16u
#define MINBLOCK 32u
#else
typedef uint32_t word_t;
#define WSIZE 4u
#define ALIGNMENT 8u
#define MINBLOCK 16u
#endif
#define DSIZE (2u * WSIZE)
#define ALIGN(n) (((n) + ALIGNMENT - 1) & ~(size_t)(ALIGNMENT - 1))
static char *heap_listp, *heap_end;
#if EXPLICIT
static char *free_listp, *mem_start_brk, *mem_brk, *mem_max_addr;
#endif
#ifdef DEBUG
static int verbose = 1;
#else
static int verbose = 0;
#endif
/* memcpy avoids aliasing/alignment-dependent typed dereferences. */
static word_t get(const void *p) { word_t w; memcpy(&w,p,WSIZE); return w; }
static void put(void *p, word_t w) { memcpy(p,&w,WSIZE); }
static char *hdr(void *bp) { return (char *)bp-WSIZE; }
static size_t size_of(void *bp) { return (size_t)(get(hdr(bp)) & ~(word_t)(ALIGNMENT-1)); }
static int allocated(void *bp) { return (int)(get(hdr(bp)) & 1); }
static char *next_block(void *bp) { return (char *)bp+size_of(bp); }
static char *prev_block(void *bp) { return (char *)bp-(get((char *)bp-DSIZE)&~(word_t)(ALIGNMENT-1)); }
static void write_block(void *bp,size_t size,int prev,int alloc) {
    word_t w=(word_t)size|(word_t)alloc;
#if EXPLICIT
    w|=(word_t)prev<<1;
#else
    (void)prev;
#endif
    put(hdr(bp),w);
    /* Allocated explicit blocks have no footer; implicit ones do. */
    if(size && (!EXPLICIT || !alloc)) put((char *)bp+size-DSIZE,w);
}
static int prev_allocated(void *bp) {
#if EXPLICIT
    return (int)((get(hdr(bp))>>1)&1);
#else
    return (int)(get((char *)bp-DSIZE)&1);
#endif
}
static void set_next_prev(void *bp,int alloc) {
#if EXPLICIT
    char *n=next_block(bp);
    word_t w=(get(hdr(n))&~(word_t)2)|((word_t)alloc<<1);
    put(hdr(n),w);
    if(size_of(n) && !allocated(n)) put(n+size_of(n)-DSIZE,w);
#else
    (void)bp; (void)alloc;
#endif
}
#if EXPLICIT
static char *pred(void *bp) { char *p; memcpy(&p,bp,sizeof p); return p; }
static char *succ(void *bp) { char *p; memcpy(&p,(char *)bp+WSIZE,sizeof p); return p; }
static void set_pred(void *bp,char *p) { memcpy(bp,&p,sizeof p); }
static void set_succ(void *bp,char *p) { memcpy((char *)bp+WSIZE,&p,sizeof p); }
static void delete_from_free_list(void *bp) {
    char *p=pred(bp),*s=succ(bp);
    if(p) set_succ(p,s); else free_listp=s;
    if(s) set_pred(s,p);
}
static void add_to_free_list(void *bp) {
    set_pred(bp,NULL); set_succ(bp,free_listp);
    if(free_listp) set_pred(free_listp,bp);
    free_listp=bp;
}
void *mm_sbrk(int incr) {
    if(incr<0 || (size_t)incr>(size_t)(mem_max_addr-mem_brk)) return (void *)-1;
    char *old=mem_brk; mem_brk+=incr; return old;
}
#endif
static void *coalesce(void *vp) {
    char *bp=vp,*n=next_block(bp);
    int pa=prev_allocated(bp),na=allocated(n);
    size_t size=size_of(bp);
    /* bp is not in the free list yet. Unlink neighbours BEFORE overwriting. */
    if(!na) {
#if EXPLICIT
        delete_from_free_list(n);
#endif
        size+=size_of(n);
    }
    if(!pa) {
        char *p=prev_block(bp);
#if EXPLICIT
        delete_from_free_list(p);
#endif
        size+=size_of(p); bp=p; pa=prev_allocated(p);
    }
    write_block(bp,size,pa,0); set_next_prev(bp,0);
#if EXPLICIT
    add_to_free_list(bp);
#endif
    return bp;
}
/* Explicit argument is WORDS, implicit argument is BYTES, as in the task. */
static void *extend_heap(size_t amount) {
#if EXPLICIT
    if(amount>SIZE_MAX/WSIZE) return NULL;
    size_t bytes=amount*WSIZE;
#else
    size_t bytes=amount;
#endif
    if(bytes>MAX_HEAP || bytes>SIZE_MAX-(CHUNKSIZE-1)) return NULL;
    bytes=(bytes+CHUNKSIZE-1)&~(size_t)(CHUNKSIZE-1);
    if((size_t)(heap_end-heap_listp)+bytes>MAX_HEAP) return NULL;
#if EXPLICIT
    char *bp=mm_sbrk((int)bytes);
#else
    /* No other sbrk user is allowed in this educational process. */
    if(sbrk(0)!=heap_end) return NULL;
    char *bp=sbrk((intptr_t)bytes);
#endif
    if(bp==(void *)-1) return NULL;
    int pa=prev_allocated(bp);
    write_block(bp,bytes,pa,0);
    write_block(bp+bytes,0,0,1);
    heap_end=bp+bytes;
    return coalesce(bp);
}
int mm_init(void) {
    if(heap_listp) return -1;
#if EXPLICIT
    _Static_assert(sizeof(void *)==8,"64-bit pointers required");
    mem_start_brk=mmap(NULL,MAX_HEAP,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
    if(mem_start_brk==MAP_FAILED) { mem_start_brk=NULL; return -1; }
    mem_brk=mem_start_brk; mem_max_addr=mem_start_brk+MAX_HEAP;
    char *base=mm_sbrk(4*WSIZE);
#else
    char *base=sbrk(0);
    size_t pad=(ALIGNMENT-((uintptr_t)base%ALIGNMENT))%ALIGNMENT;
    if(pad && sbrk((intptr_t)pad)==(void *)-1) return -1;
    base=sbrk(4*WSIZE);
#endif
    if(base==(void *)-1) return -1;
    put(base,0); heap_listp=base+2*WSIZE;
    put(base+WSIZE,DSIZE|3); put(base+2*WSIZE,DSIZE|3);
    /* Corrected template typo: epilogue is at +3*WSIZE, not base. */
    put(base+3*WSIZE,3); heap_end=base+4*WSIZE;
    return extend_heap(EXPLICIT?CHUNKSIZE/WSIZE:CHUNKSIZE)?0:-1;
}
#if EXPLICIT
static void *find_fit_first(size_t size) {
    for(char *p=free_listp;p;p=succ(p)) if(size_of(p)>=size) return p;
    return NULL;
}
static void *find_fit_best(size_t size) {
    char *best=NULL;
    for(char *p=free_listp;p;p=succ(p))
        if(size_of(p)>=size && (!best || size_of(p)<size_of(best))) best=p;
    return best;
}
#else
static void *find_fit(size_t size) {
    for(char *p=next_block(heap_listp);size_of(p);p=next_block(p))
        if(!allocated(p) && size_of(p)>=size) return p;
    return NULL;
}
#endif
static void place(void *vp,size_t asize) {
    char *bp=vp; size_t size=size_of(bp); int pa=prev_allocated(bp);
#if EXPLICIT
    delete_from_free_list(bp);
#endif
    if(size-asize>=MINBLOCK) {
        write_block(bp,asize,pa,1);
        char *rest=bp+asize; write_block(rest,size-asize,1,0);
        set_next_prev(rest,0);
#if EXPLICIT
        add_to_free_list(rest);
#endif
    } else { write_block(bp,size,pa,1); set_next_prev(bp,1); }
}
static void print_heap_blocks(int v,const char *func) {
    if(!v) return;
    printf("\n=== %s (%s) ===\n",func,EXPLICIT?"explicit":"implicit");
    for(char *p=heap_listp;;p=next_block(p)) {
        printf("Address=%p Size=%zu Allocated=%d",(void *)p,size_of(p),allocated(p));
#if EXPLICIT
        printf(" PrevAllocated=%d",prev_allocated(p));
#endif
        putchar('\n'); if(!size_of(p)) break;
    }
}
static void check_heap(void) {
    size_t free_count=0; int pa=1;
    for(char *p=next_block(heap_listp);;p=next_block(p)) {
        assert(p<=heap_end);
#if EXPLICIT
        assert(prev_allocated(p)==pa);
#endif
        size_t n=size_of(p);
        if(!n) { assert(p==heap_end && allocated(p)); break; }
        assert((uintptr_t)p%ALIGNMENT==0 && n>=MINBLOCK && n%ALIGNMENT==0);
        assert(n<=(size_t)(heap_end-p));
        if(!EXPLICIT || !allocated(p)) assert(get(hdr(p))==get(p+n-DSIZE));
        if(!allocated(p)) { assert(pa); free_count++; }
        pa=allocated(p);
    }
#if EXPLICIT
    size_t count=0; char *last=NULL;
    for(char *p=free_listp;p;p=succ(p)) {
        assert(++count<=free_count); assert(!allocated(p) && pred(p)==last);
        int found=0;
        for(char *q=next_block(heap_listp);size_of(q);q=next_block(q)) if(q==p) found=1;
        assert(found); last=p;
    }
    assert(count==free_count);
#else
    (void)free_count;
#endif
}
void *mm_malloc(size_t size) {
    if(!size || size>MAX_HEAP-DSIZE) return NULL;
    if(!heap_listp && mm_init()<0) return NULL;
    size_t asize=ALIGN(size+(EXPLICIT?WSIZE:DSIZE));
    if(asize<MINBLOCK) asize=MINBLOCK;
    void *bp;
#if EXPLICIT
#ifdef BEST_FIT
    bp=find_fit_best(asize); (void)find_fit_first;
#else
    bp=find_fit_first(asize); (void)find_fit_best;
#endif
#else
    bp=find_fit(asize);
#endif
    if(!bp) bp=extend_heap(EXPLICIT?asize/WSIZE:asize);
    if(!bp) return NULL;
    place(bp,asize); check_heap(); print_heap_blocks(verbose,"mm_malloc"); return bp;
}
void mm_free(void *bp) {
    if(!bp) return;
    /* As with free(), non-owned pointers and double free are invalid inputs. */
    assert(allocated(bp));
    write_block(bp,size_of(bp),prev_allocated(bp),0);
    coalesce(bp); check_heap(); print_heap_blocks(verbose,"mm_free");
}
#if EXPLICIT
void *mm_realloc(void *ptr,size_t size) {
    if(!ptr) return mm_malloc(size);
    if(!size) { mm_free(ptr); return NULL; }
    if(size>MAX_HEAP-DSIZE) return NULL;
    size_t old=size_of(ptr),asize=ALIGN(size+WSIZE);
    if(asize<MINBLOCK) asize=MINBLOCK;
    if(asize<=old) {
        if(old-asize>=MINBLOCK) {
            int pa=prev_allocated(ptr); write_block(ptr,asize,pa,1);
            char *rest=(char *)ptr+asize; write_block(rest,old-asize,1,0); coalesce(rest);
        }
        check_heap(); return ptr;
    }
    void *p=mm_malloc(size);
    if(!p) return NULL; /* Failure leaves ptr and its data intact. */
    memcpy(p,ptr,old-WSIZE); mm_free(ptr); return p;
}
#endif
static void selftest(void) {
    verbose=0;
    assert(mm_malloc(0)==NULL && mm_malloc(SIZE_MAX)==NULL); mm_free(NULL);
    /* Exercise neither/left/right/both-neighbour coalescing. */
    for(int pattern=0;pattern<4;pattern++) {
        void *a=mm_malloc(64),*b=mm_malloc(64),*c=mm_malloc(64),*d=mm_malloc(64);
        assert(a&&b&&c&&d);
        if(pattern&1) mm_free(a);
        if(pattern&2) mm_free(c);
        mm_free(b);
        if(!(pattern&1)) mm_free(a);
        if(!(pattern&2)) mm_free(c);
        mm_free(d); check_heap();
    }
#if EXPLICIT
    unsigned char *p=mm_realloc(NULL,100); assert(p);
    for(int i=0;i<100;i++) p[i]=(unsigned char)i;
    unsigned char *q=mm_realloc(p,9000); assert(q);
    for(int i=0;i<100;i++) assert(q[i]==(unsigned char)i);
    assert(mm_realloc(q,SIZE_MAX)==NULL && q[99]==99);
    p=mm_realloc(q,20); assert(p==q);
    for(int i=0;i<20;i++) assert(p[i]==(unsigned char)i);
    assert(mm_realloc(p,0)==NULL);
#endif
    /* Deterministic mixed allocations: check every live byte before freeing. */
    void *slots[32]={0}; size_t sizes[32]={0}; uint32_t seed=7;
    for(int k=0;k<3000;k++) {
        seed=seed*1664525u+1013904223u; unsigned i=(seed>>16)%32;
        if(slots[i]) {
            for(size_t j=0;j<sizes[i];j++) assert(((unsigned char *)slots[i])[j]==i+1);
            mm_free(slots[i]); slots[i]=NULL;
        } else {
            sizes[i]=(seed%6000)+1; slots[i]=mm_malloc(sizes[i]); assert(slots[i]);
            memset(slots[i],(int)i+1,sizes[i]);
        }
    }
    for(int i=0;i<32;i++) mm_free(slots[i]);
    check_heap();
    /* Exhaust the bounded heap, then verify all blocks can be reclaimed. */
    void *large[100]; size_t n=0;
    while(n<100 && (large[n]=mm_malloc(65536))!=NULL) n++;
    assert(n<100 && n>0);
    while(n) mm_free(large[--n]);
    check_heap();
    printf("PASS: alignment, split, four coalesce cases, 3000 mixed operations, exhaustion%s\n",EXPLICIT?", realloc preservation/failure":"");
}
int main(int argc,char **argv) {
    /* Avoid stdio allocating from the process brk during implicit tests. */
    setvbuf(stdout,NULL,_IONBF,0);
    if(mm_init()<0) { puts("Initialization failed"); return 1; }
    if(argc>1 && strcmp(argv[1],"--test")==0) selftest();
    else {
        void *p1=mm_malloc(8000),*p2=mm_malloc(200),*p3=mm_malloc(50);
        assert(p1&&p2&&p3); mm_free(p2);
        void *p4=mm_malloc(150); assert(p4);
        mm_free(p1); mm_free(p3); mm_free(p4);
        check_heap(); puts("PASS: supplied allocation/free sequence; all blocks coalesced.");
    }
#if EXPLICIT
    munmap(mem_start_brk,MAX_HEAP);
#endif
    return 0;
}

