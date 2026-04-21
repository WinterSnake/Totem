#include <stdio.h>
#include "totem.h"

typedef struct TestStruct { int32_t value; } TestStruct;

int main()
{
	uint8_t buffer[2048];
	FixedBufferAllocator fb = FixedBufferAllocator_Init(buffer, 2048);
	printf("[FBA]Start: %p;Capacity: %lu\n", fb.start, fb.capacity);
	// Allocation
	printf("Alloc..\n");
	allocator_t fbAlloc = FixedBufferAllocator_GetAllocator(&fb);
	printf("[FBA]Offset: %lu;Current: %p\n", fb.offset, fb.start + fb.offset);
	slice_t alloc1 = Totem_Alloc(&fbAlloc, 25, 0);
	printf("[Alloc1]ptr: %p;Len: %lu\n", alloc1.data, alloc1.length);
	printf("[FBA]Offset: %lu;Current: %p\n", fb.offset, fb.start + fb.offset);
	slice_t alloc2 = Totem_Alloc_T(&fbAlloc, uint32_t, 20);
	printf("[Alloc2]ptr: %p;Len: %lu\n", alloc2.data, alloc2.length);
	printf("[FBA]Offset: %lu;Current: %p\n", fb.offset, fb.start + fb.offset);
	TestStruct* alloc3 = Totem_Create_T(&fbAlloc, TestStruct);
	alloc3->value = 125;
	printf("[Alloc3]ptr: %p;Value: %d\n", alloc3, alloc3->value);
	printf("[FBA]Offset: %lu;Current: %p\n", fb.offset, fb.start + fb.offset);
	Totem_Delete_T(&fbAlloc, &alloc3, TestStruct);
	printf("[Alloc3]ptr: %p\n", alloc3);
	printf("[FBA]Offset: %lu;Current: %p\n", fb.offset, fb.start + fb.offset);
	// Resize
	printf("Resize..\n");
	bool isResized = Totem_Resize(&fbAlloc, &alloc1, 29, 0);
	printf("[Alloc1]resized: %i, ptr: %p;Len: %lu\n", isResized, alloc1.data, alloc1.length);
	isResized = Totem_Resize_T(&fbAlloc, &alloc2, uint32_t, 40);
	printf("[Alloc2]resized: %i, ptr: %p;Len: %lu\n", isResized, alloc2.data, alloc2.length);
	isResized = Totem_Resize(&fbAlloc, &alloc1, 15, 0);
	printf("[Alloc1]resized: %i, ptr: %p;Len: %lu\n", isResized, alloc1.data, alloc1.length);
	// Free
	printf("Free..\n");
	Totem_Free_T(&fbAlloc, &alloc2, uint32_t);
	printf("[Alloc2]ptr: %p;Len: %lu\n", alloc2.data, alloc2.length);
	printf("[FBA]Offset: %lu;Current: %p\n", fb.offset, fb.start + fb.offset);
	Totem_Free(&fbAlloc, &alloc1, 0);
	printf("[Alloc1]ptr: %p;Len: %lu\n", alloc1.data, alloc1.length);
	printf("[FBA]Offset: %lu;Current: %p\n", fb.offset, fb.start + fb.offset);
	// Iteration
	FixedBufferAllocator_Reset(&fb);
	alloc1 = Totem_Alloc_T(&fbAlloc, uint64_t, 13);
	for (size_t i = 0; i < Totem_Count_T(alloc1, uint64_t); ++i)
		*Totem_Index_T(alloc1, uint64_t, i) = 1 << i;
	for (size_t i = 0; i < Totem_Count_T(alloc1, uint64_t); ++i)
		printf("[%lu]Value: %lu\n", i, *Totem_Index_ConstT(alloc1, uint64_t, i));
	return 0;
}
