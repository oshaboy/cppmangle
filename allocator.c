#include "allocator.h"
#define INITIAL_CAPACITY 8192
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
void * bump_alloc(BumpAllocator * alloc, size_t n){
	if (!n) return NULL;
	if (alloc->buffer == NULL){
		*alloc = (BumpAllocator){
			.buffer=malloc(INITIAL_CAPACITY),
			.capacity=INITIAL_CAPACITY,
		};
		memset(alloc->buffer, 0xdd, alloc->capacity);
	}

	while (alloc->capacity<alloc->index+n){
		const size_t prev_capacity=alloc->capacity;
		alloc->capacity*=4;
		alloc->buffer=realloc(alloc->buffer, alloc->capacity);
		memset(alloc->buffer+prev_capacity, 0xdd, alloc->capacity-prev_capacity);
	}
	void * result=alloc->buffer+alloc->index;
	if(memcmp(result, "\xdd\xdd\xdd\xdd",4)){
		fprintf(stderr, "Buffer Overrun Detected\n");
		exit(8);
	}
	alloc->index+=n;
	return result;

}
