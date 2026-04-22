#include <stdio.h>
#include "totem.h"

int main()
{
	LinearAllocator l = LinearAllocator_Init(PageAllocator);
	// Allocation
	printf("Alloc..\n");
	allocator_t lAlloc = LinearAllocator_GetAllocator(&l);
	printf("[LA]Cursor{Start: %p;Capacity: %lu}; Position{ptr: %p, offset: %lu}\n", l.allocation.data, l.allocation.length, l.allocation.data + l.offset, l.offset);
	slice_t alloc1 = Totem_Alloc_T(&lAlloc, uint32_t, 32);
	printf("[Alloc1]ptr: %p;Len: %lu\n", alloc1.data, alloc1.length);
	printf("[LA]Cursor{Start: %p;Capacity: %lu}; Position{ptr: %p, offset: %lu}\n", l.allocation.data, l.allocation.length, l.allocation.data + l.offset, l.offset);
	slice_t alloc2 = Totem_Alloc_T(&lAlloc, uint64_t, 32);
	printf("[Alloc2]ptr: %p;Len: %lu\n", alloc2.data, alloc2.length);
	printf("[LA]Cursor{Start: %p;Capacity: %lu}; Position{ptr: %p, offset: %lu}\n", l.allocation.data, l.allocation.length, l.allocation.data + l.offset, l.offset);
	slice_t alloc3 = Totem_Alloc_T(&lAlloc, uint64_t, 464);
	*Totem_Index_T(alloc3, uint64_t, 463) = 0xDEADBEEF;
	printf("[Alloc3]ptr: %p;Len: %lu\n", alloc3.data, alloc3.length);
	printf("[LA]Cursor{Start: %p;Capacity: %lu}; Position{ptr: %p, offset: %lu}\n", l.allocation.data, l.allocation.length, l.allocation.data + l.offset, l.offset);
	printf("[Alloc3]Value: %lX\n", *Totem_Index_T(alloc3, uint64_t, 463));
	slice_t alloc4 = Totem_Alloc_T(&lAlloc, uint64_t, 100);
	printf("[Alloc4]ptr: %p;Len: %lu\n", alloc4.data, alloc4.length);
	printf("[LA]Cursor{Start: %p;Capacity: %lu}; Position{ptr: %p, offset: %lu}\n", l.allocation.data, l.allocation.length, l.allocation.data + l.offset, l.offset);
	// Resize
	printf("Resize..\n");
	bool isResized = Totem_Resize_T(&lAlloc, &alloc2, uint64_t, 64);
	printf("[Alloc2]resized: %i, ptr: %p;Len: %lu\n", isResized, alloc2.data, alloc2.length);
	isResized = Totem_Resize_T(&lAlloc, &alloc3, uint64_t, 300);
	printf("[Alloc3]resized: %i, ptr: %p;Len: %lu\n", isResized, alloc3.data, alloc3.length);
	alloc4 = Totem_Alloc_T(&lAlloc, uint64_t, 100);
	printf("[Alloc4]ptr: %p;Len: %lu\n", alloc4.data, alloc4.length);
	printf("[LA]Cursor{Start: %p;Capacity: %lu}; Position{ptr: %p, offset: %lu}\n", l.allocation.data, l.allocation.length, l.allocation.data + l.offset, l.offset);
	isResized = Totem_Resize_T(&lAlloc, &alloc2, uint64_t, 16);
	printf("[Alloc2]resized: %i, ptr: %p;Len: %lu\n", isResized, alloc2.data, alloc2.length);
	// Free
	printf("Free..\n");
	Totem_Free_T(&lAlloc, &alloc4, uint64_t);
	printf("[Alloc4]ptr: %p;Len: %lu\n", alloc4.data, alloc4.length);
	printf("[LA]Cursor{Start: %p;Capacity: %lu}; Position{ptr: %p, offset: %lu}\n", l.allocation.data, l.allocation.length, l.allocation.data + l.offset, l.offset);
	Totem_Free_T(&lAlloc, &alloc3, uint64_t);
	printf("[Alloc3]ptr: %p;Len: %lu\n", alloc3.data, alloc3.length);
	printf("[LA]Cursor{Start: %p;Capacity: %lu}; Position{ptr: %p, offset: %lu}\n", l.allocation.data, l.allocation.length, l.allocation.data + l.offset, l.offset);
	Totem_Free_T(&lAlloc, &alloc2, uint64_t);
	printf("[Alloc2]ptr: %p;Len: %lu\n", alloc2.data, alloc2.length);
	printf("[LA]Cursor{Start: %p;Capacity: %lu}; Position{ptr: %p, offset: %lu}\n", l.allocation.data, l.allocation.length, l.allocation.data + l.offset, l.offset);
	// Deinit
	printf("Deinit..\n");
	LinearAllocator_Deinit(&l);
	printf("[LA]Cursor{Start: %p;Capacity: %lu}; Position{ptr: %p, offset: %lu}\n", l.allocation.data, l.allocation.length, l.allocation.data + l.offset, l.offset);
	// Init w/ Capacity
	if (!LinearAllocator_Init_T(&l, PageAllocator, uint64_t, 800))
		return 1;
	printf("Alloc..\n");
	printf("[LA]Cursor{Start: %p;Capacity: %lu}; Position{ptr: %p, offset: %lu}\n", l.allocation.data, l.allocation.length, l.allocation.data + l.offset, l.offset);
	alloc1 = Totem_Alloc_T(&lAlloc, uint32_t, 32);
	printf("[Alloc1]ptr: %p;Len: %lu\n", alloc1.data, alloc1.length);
	printf("[LA]Cursor{Start: %p;Capacity: %lu}; Position{ptr: %p, offset: %lu}\n", l.allocation.data, l.allocation.length, l.allocation.data + l.offset, l.offset);
	printf("Deinit..\n");
	LinearAllocator_Deinit(&l);
	printf("[LA]Cursor{Start: %p;Capacity: %lu}; Position{ptr: %p, offset: %lu}\n", l.allocation.data, l.allocation.length, l.allocation.data + l.offset, l.offset);
	return 0;
}
