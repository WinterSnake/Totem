#include <stdio.h>
#include "totem.h"

int main()
{
	// Allocation
	printf("Alloc..\n");
	slice_t alloc1 = Totem_Alloc(PageAllocator, 1024 * 8, 8);
	printf("[Alloc1]ptr: %p;Len: %lu\n", alloc1.data, alloc1.length);
	// Resize
	printf("Resize..\n");
	bool isResized = Totem_Resize(PageAllocator, &alloc1, 1024, 8);
	printf("[Alloc1]resized: %i, ptr: %p;Len: %lu\n", isResized, alloc1.data, alloc1.length);
	isResized = Totem_Resize(PageAllocator, &alloc1, 1024 * 4, 8);
	printf("[Alloc1]resized: %i, ptr: %p;Len: %lu\n", isResized, alloc1.data, alloc1.length);
	isResized = Totem_Resize(PageAllocator, &alloc1, 1024 * 9, 8);
	printf("[Alloc1]resized: %i, ptr: %p;Len: %lu\n", isResized, alloc1.data, alloc1.length);
	// Remap
	printf("Remap..\n");
	*(uint64_t*)alloc1.data = 0xDEADBEEF;
	alloc1 = Totem_Remap(PageAllocator, &alloc1, 1024 * 9, 8);
	printf("[Alloc1]ptr: %p;Len: %lu\n", alloc1.data, alloc1.length);
	printf("Value: %lX\n", *(uint64_t*)alloc1.data);
	// Free
	printf("Free..\n");
	Totem_Free(PageAllocator, &alloc1, 8);
	printf("[Alloc1]ptr: %p;Len: %lu\n", alloc1.data, alloc1.length);
	return 0;
}
