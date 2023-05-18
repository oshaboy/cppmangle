#ifndef ALLOCATOR_H
#define ALLOCATOR_H
#include <stdlib.h>
typedef struct {
	void * buffer;
	size_t index;
	size_t capacity;
} BumpAllocator;
void * bump_alloc(BumpAllocator * alloc,size_t n);
inline static void bump_free(BumpAllocator * ALLOC) {free(ALLOC->buffer);}
#endif