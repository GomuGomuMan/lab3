/* $begin mallocinterface */
extern int mm_init(void);
extern void *mm_malloc (size_t size);
extern void mm_free (void *ptr);
/* $end mallocinterface */

extern void set_next_fit(int b);
extern void print_blocks();
extern void write_block(void* memLoc, char byte, int numCopies);
extern void print_block(void* memLoc, int numBytes);

extern void *mm_realloc(void *ptr, size_t size);
extern void *mm_calloc (size_t nmemb, size_t size);
extern void mm_checkheap(int verbose);
