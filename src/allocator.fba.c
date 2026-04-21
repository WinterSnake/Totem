/*
	Totem Allocator Library
	- Fixed Buffer

	Written By: Ryan Smith
*/

#include "totem.h"

AllocatorContext_T(FixedBufferAllocator) FixedBufferContext;
extern size_t Totem_alignPointer(void**, const uint8_t);
static bool isLastSlice(FixedBufferContext, const slice_t);

// VTable
static void* fbAlloc(allocator_ctx_t context, size_t length, const uint8_t alignment)
{
	AllocatorContext_As(context, FixedBufferContext, ctx);
	void* aligned_pointer = ctx->start + ctx->offset;
	size_t padding = Totem_alignPointer(&aligned_pointer, alignment);
	if (ctx->offset + padding + length > ctx->capacity)
		return NULL;
	ctx->offset += padding + length;
	return aligned_pointer;
}
static bool fbResize(allocator_ctx_t context, slice_t* const memory, size_t new_length, const uint8_t alignment)
{
	(void)alignment;
	AllocatorContext_As(context, FixedBufferContext, ctx);
	if (!isLastSlice(ctx, *memory))
	{
		if (new_length > memory->length)
			return false;
		memory->length = new_length;
		return true;
	}
	size_t previous = memory->data - ctx->start;
	if (previous + new_length > ctx->capacity)
		return false;
	ctx->offset = previous + new_length;
	memory->length = new_length;
	return true;
}
static void fbFree(allocator_ctx_t context, const slice_t memory, const uint8_t alignment)
{
	(void)alignment;
	AllocatorContext_As(context, FixedBufferContext, ctx);
	if (isLastSlice(ctx, memory))
		ctx->offset = memory.data - ctx->start;
}
static const struct Totem_AllocatorVTable fbVTable = {
	.alloc=fbAlloc,
	.resize=fbResize,
	.remap=NULL,
	.free=fbFree,
};
// API
FixedBufferAllocator FixedBufferAllocator_Init(void* const memory, size_t capacity)
{
	return (FixedBufferAllocator){
		.start=memory,
		.offset=0,
		.capacity=capacity,
	};
}
allocator_t FixedBufferAllocator_GetAllocator(FixedBufferAllocator* const fba)
{
	return (allocator_t){
		.context=fba,
		.vtable=&fbVTable,
	};
}
void FixedBufferAllocator_Reset(FixedBufferAllocator* const fba)
{
	fba->offset = 0;
}
// Helpers
static bool isLastSlice(FixedBufferContext ctx, const slice_t slice)
{
	return ctx->start + ctx->offset == slice.data + slice.length;
}
