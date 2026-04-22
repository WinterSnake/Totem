/*
	Totem Allocator Library
	- Linear

	Written By: Ryan Smith
*/

#include "totem.h"

AllocatorContext_T(LinearAllocator) LinearContext;
extern size_t Totem_alignPointer(void**, const uint8_t);
static bool isLastSlice(LinearContext, const slice_t);
static bool canFitOrResize(LinearContext, const size_t);

// VTable
static void* Totem_LinearAlloc(allocator_ctx_t context, size_t length, const uint8_t alignment)
{
	AllocatorContext_As(context, LinearContext, ctx);
	// Initialize
	if (ctx->allocation.data == NULL)
	{
		void* result = Totem_RawAlloc(ctx->parent, length, alignment);
		if (result == NULL)
			return NULL;
		ctx->alignment = alignment;
		ctx->allocation = (slice_t){
			.data=result,
			.length=length,
		};
	}
	// Carve
	void* aligned_pointer = ctx->allocation.data + ctx->offset;
	size_t padding = Totem_alignPointer(&aligned_pointer, alignment);
	size_t required_size = ctx->offset + padding + length;
	if (!canFitOrResize(ctx, required_size))
		return NULL;
	ctx->offset = required_size;
	return aligned_pointer;
}
static bool Totem_LinearResize(allocator_ctx_t context, slice_t* const memory, size_t new_length, const uint8_t alignment)
{
	(void)alignment;
	AllocatorContext_As(context, LinearContext, ctx);
	if (!isLastSlice(ctx, *memory))
	{
		if (new_length > memory->length)
			return false;
		memory->length = new_length;
		return true;
	}
	size_t previous = memory->data - ctx->allocation.data;
	size_t required_size = previous + new_length;
	if (!canFitOrResize(ctx, required_size))
		return false;
	ctx->offset = required_size;
	memory->length = new_length;
	return true;
}
static void Totem_LinearFree(allocator_ctx_t context, const slice_t memory, const uint8_t alignment)
{
	(void)alignment;
	AllocatorContext_As(context, LinearContext, ctx);
	if (isLastSlice(ctx, memory))
		ctx->offset = memory.data - ctx->allocation.data;
}
static const struct Totem_AllocatorVTable LinearVTable = {
	.alloc=Totem_LinearAlloc,
	.resize=Totem_LinearResize,
	.remap=NULL,
	.free=Totem_LinearFree,
};
// API
LinearAllocator LinearAllocator_Init(allocator_t* const parent)
{
	return (LinearAllocator){
		.parent=parent,
		.allocation={0},
		.offset=0,
		.alignment=0,
	};
}
bool LinearAllocator_InitCapacity(LinearAllocator* const la, allocator_t* const parent, const size_t capacity, const uint8_t alignment)
{
	void* result = Totem_RawAlloc(parent, capacity, alignment);
	if (result == NULL)
		return false;
	*la = (LinearAllocator){
		.parent=parent,
		.allocation={
			.data=result,
			.length=capacity,
		},
		.offset=0,
		.alignment=alignment,
	};
	return true;
}
allocator_t LinearAllocator_GetAllocator(LinearAllocator* const la)
{
	return (allocator_t){
		.context=la,
		.vtable=&LinearVTable,
	};
}
void LinearAllocator_Reset(LinearAllocator* const la)
{
	la->offset = 0;
}
void LinearAllocator_Deinit(LinearAllocator* const la)
{
	if (la->allocation.data != NULL)
		Totem_RawFree(la->parent, la->allocation, la->alignment);
	*la = (LinearAllocator){
		.parent=NULL,
		.allocation={0},
		.offset=0,
		.alignment=0,
	};
}
// Helpers
static bool isLastSlice(LinearContext ctx, const slice_t slice)
{
	return ctx->allocation.data + ctx->offset == slice.data + slice.length;
}
static bool canFitOrResize(LinearContext ctx, const size_t length)
{
	if (length > ctx->allocation.length)
	{
		if (!Totem_RawResize(ctx->parent, &ctx->allocation, length, ctx->alignment))
			return false;
	}
	return true;
}
