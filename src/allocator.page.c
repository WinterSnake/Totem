/*
	Totem Allocator Library
	- Page

	Written By: Ryan Smith
*/

#define _GNU_SOURCE
#include <assert.h>
#include <sys/mman.h>
#include <unistd.h>
#include "totem.h"

extern uint8_t Totem_log2Alignment(const size_t alignment);
extern size_t Totem_alignForward(const size_t, const uint8_t);
static size_t getPageSize(void);
static size_t getRoundedPage(const size_t, const uint8_t);

// VTable
static void* Totem_PageAlloc(allocator_ctx_t context, size_t length, const uint8_t alignment)
{
	(void)context;
	length = getRoundedPage(length, alignment);
	void* page = mmap(
		NULL,
		length,
		PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS,
		-1,
		0
	);
	if (page == MAP_FAILED)
		return NULL;
	return page;
}
static bool Totem_PageResize(allocator_ctx_t context, slice_t* const memory, size_t new_length, const uint8_t alignment)
{
	(void)context;
	size_t old_length = getRoundedPage(memory->length, alignment);
	size_t act_length = getRoundedPage(new_length, alignment);
	// Same page count
	if (old_length == act_length)
	{
		memory->length = new_length;
		return true;
	}
	// Smaller page count
	else if (old_length > act_length)
	{
		munmap(memory->data + act_length, old_length - act_length);
		memory->length = new_length;
		return true;
	}
	// Larger page count
	void* page = mremap(memory->data, old_length, act_length, 0);
	if (page == MAP_FAILED)
		return false;
	assert(memory->data == page);
	memory->length = new_length;
	return true;
}
static void* Totem_PageRemap(allocator_ctx_t context, const slice_t memory, size_t new_length, const uint8_t alignment)
{
	(void)context;
	size_t old_length = getRoundedPage(memory.length, alignment);
	new_length = getRoundedPage(new_length, alignment);
	void* page = mremap(memory.data, old_length, new_length, MREMAP_MAYMOVE);
	if (page == MAP_FAILED)
		return NULL;
	return page;
}
static void Totem_PageFree(allocator_ctx_t context, const slice_t memory, const uint8_t alignment)
{
	(void)context;
	size_t actualLength = getRoundedPage(memory.length, alignment);
	int result = munmap(memory.data, actualLength);
	assert(result == 0);
}
static const struct Totem_AllocatorVTable PageVTable = {
	.alloc=Totem_PageAlloc,
	.resize=Totem_PageResize,
	.remap=Totem_PageRemap,
	.free=Totem_PageFree,
};
// API
static allocator_t _PageAllocator = (allocator_t){
	.context=NULL,
	.vtable=&PageVTable,
};
allocator_t* const PageAllocator = &_PageAllocator;
// Helpers
static size_t getPageSize(void)
{
	static long pageSize = 0;
	if (pageSize == 0)
	{
		pageSize = sysconf(_SC_PAGESIZE);
		assert(pageSize > 0);
	}
	return (size_t)pageSize;
}
static size_t getRoundedPage(const size_t length, uint8_t alignment)
{
	uint8_t pageAlignment = Totem_log2Alignment(getPageSize());
	alignment = (alignment > pageAlignment) ? alignment : pageAlignment;
	return Totem_alignForward(length, alignment);
}
