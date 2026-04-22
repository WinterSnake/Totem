/*
	Totem Allocator Library

	Written By: Ryan Smith
*/

#include <assert.h>
#include <string.h>
#include "totem.h"

// API: Raw
void* Totem_RawAlloc(allocator_t* const allocator, const size_t length, const uint8_t alignment)
{
	return allocator->vtable->alloc(allocator->context, length, alignment);
}
bool Totem_RawResize(allocator_t* const allocator, slice_t* const memory, const size_t new_length, const uint8_t alignment)
{
	return allocator->vtable->resize(allocator->context, memory, new_length, alignment);
}
void* Totem_RawRemap(allocator_t* const allocator, const slice_t memory, const size_t new_length, const uint8_t alignment)
{
	if (allocator->vtable->remap == NULL)
		return NULL;
	return allocator->vtable->remap(allocator->context, memory, new_length, alignment);
}
void Totem_RawFree(allocator_t* const allocator, const slice_t memory, const uint8_t alignment)
{
	allocator->vtable->free(allocator->context, memory, alignment);
}
// API: Core
slice_t Totem_Alloc(allocator_t* const allocator, const size_t length, const uint8_t alignment)
{
	void* data = Totem_RawAlloc(allocator, length, alignment);
	return (slice_t){
		.data=data,
		.length=data == NULL ? 0 : length,
	};
}
bool Totem_Resize(allocator_t* const allocator, slice_t* const memory, const size_t new_length, const uint8_t alignment)
{
	if (new_length == 0)
	{
		Totem_Free(allocator, memory, alignment);
		return true;
	}
	else if (memory->length == new_length)
		return true;
	return Totem_RawResize(allocator, memory, new_length, alignment);
}
slice_t Totem_Remap(allocator_t* const allocator, slice_t* const memory, const size_t new_length, const uint8_t alignment)
{
	// Resize
	if (Totem_Resize(allocator, memory, new_length, alignment))
	{
		slice_t result = *memory;
		*memory = (slice_t){0};
		return result;
	}
	// Remap
	void* result = Totem_RawRemap(allocator, *memory, new_length, alignment);
	if (result != NULL)
	{
		*memory = (slice_t){0};
		return (slice_t){
			.data=result,
			.length=new_length,
		};
	}
	return (slice_t){0};
}
slice_t Totem_Realloc(allocator_t* const allocator, slice_t* const memory, const size_t new_length, const uint8_t alignment)
{
	// Resize OR Remap
	slice_t result = Totem_Remap(allocator, memory, new_length, alignment);
	if (result.data != NULL)
		return result;
	// Allocate + Copy + Free
	result = Totem_Alloc(allocator, new_length, alignment);
	if (result.data != NULL)
	{
		size_t length = (memory->length < new_length) ? memory->length : new_length;
		memcpy(result.data, memory->data, length);
		Totem_Free(allocator, memory, alignment);
	}
	return result;
}
void Totem_Free(allocator_t* const allocator, slice_t* const memory, const uint8_t alignment)
{
	if (memory->data == NULL) return;
	Totem_RawFree(allocator, *memory, alignment);
	*memory = (slice_t){0};
}
// Helpers
uint8_t Totem_log2Alignment(const size_t alignment)
{
	assert(alignment > 0 && ((alignment & (alignment - 1)) == 0));
	return (uint8_t)__builtin_ctzll(alignment);
}
size_t Totem_alignForward(const size_t size, const uint8_t alignment)
{
	size_t mask = (1ull << alignment) - 1;
	return (size + mask) & ~mask;
}
size_t Totem_alignPointer(void** origin, const uint8_t alignment)
{
	size_t mask = (1ull << alignment) - 1;
	uintptr_t raw = (uintptr_t)*origin;
	uintptr_t aligned = (raw + mask) & ~mask;
	size_t offset = aligned - raw;
	*origin = (void*)aligned;
	return offset;
}
