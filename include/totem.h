/*
	Totem Allocator Library

	A library for using and creating custom allocators
	akin to those you would find in Zig to be used in C.
	Has several built-in allocators in addition to defining
	the interface to create your own allocators should the
	library itself not include or have one that fits your need.

	Written By: Ryan Smith
*/

#ifndef _TOTEM_H
#define _TOTEM_H

#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/// Core
typedef struct
{
	void* data;
	size_t length;
} slice_t;

typedef void* const allocator_ctx_t;
#define AllocatorContext_T(type) typedef type* const
#define AllocatorContext_As(ctx, type, n) type n = (type)(ctx);

struct Totem_AllocatorVTable
{
	// param: context, length, alignment (log2)
	void* (*alloc)(allocator_ctx_t, size_t, const uint8_t);
	// param: context, slice, new length, alignment (log2)
	bool (*resize)(allocator_ctx_t, slice_t* const, size_t, const uint8_t);
	// param: context, slice, new length, alignment (log2)
	void* (*remap)(allocator_ctx_t, const slice_t, size_t, const uint8_t);
	// param: context, slice, alignment (log2)
	void (*free)(allocator_ctx_t, const slice_t, const uint8_t);
};

typedef struct
{
	allocator_ctx_t context;
	const struct Totem_AllocatorVTable* const vtable;
} allocator_t;

/// API: Raw
void* Totem_RawAlloc(allocator_t* const allocator, const size_t length, const uint8_t alignment);
bool  Totem_RawResize(allocator_t* const allocator, slice_t* const memory, const size_t new_length, const uint8_t alignment);
void* Totem_RawRemap(allocator_t* const allocator, const slice_t memory, const size_t new_length, const uint8_t alignment);
void  Totem_RawFree(allocator_t* const allocator, const slice_t memory, const uint8_t alignment);
/// API: Core
#define Totem_alignof(type) (uint8_t)__builtin_ctzll(alignof(type))
#define Totem_Count_T(slice, type) ((slice).length / sizeof(type))
#define Totem_Index_T(slice, type, i) ((type*)((slice).data) + (i))
#define Totem_Index_ConstT(slice, type, i) ((const type*)((slice).data) + (i))
slice_t Totem_Alloc(allocator_t* const allocator, const size_t length, const uint8_t alignment);
#define Totem_Alloc_T(alloc, type, count) Totem_Alloc((alloc), sizeof(type) * (count), Totem_alignof(type))
#define Totem_Create_T(alloc, type) ((type*)(Totem_Alloc((alloc), sizeof(type), Totem_alignof(type))).data)
bool    Totem_Resize(allocator_t* const allocator, slice_t* const memory, const size_t new_length, const uint8_t alignment);
#define Totem_Resize_T(alloc, slice, type, count) Totem_Resize((alloc), (slice), sizeof(type) * (count), Totem_alignof(type))
slice_t Totem_Remap(allocator_t* const allocator, slice_t* const memory, const size_t new_length, const uint8_t alignment);
#define Totem_Remap_T(alloc, slice, type, count) Totem_Remap((alloc), (slice), sizeof(type) * (count), Totem_alignof(type))
slice_t Totem_Realloc(allocator_t* const allocator, slice_t* const memory, const size_t new_length, const uint8_t alignment);
#define Totem_Realloc_T(alloc, slice, type, count) Totem_Realloc((alloc), (slice), sizeof(type) * (count), Totem_alignof(type))
void    Totem_Free(allocator_t* const allocator, slice_t* const memory, const uint8_t alignment);
#define Totem_Free_T(alloc, slice, type) Totem_Free((alloc), (slice), Totem_alignof(type))
#define Totem_Delete_T(alloc, value, type) \
	do {\
		Totem_Free((alloc), &(slice_t){.data=*(value), .length=sizeof(type)}, Totem_alignof(type)); \
		*(value) = NULL; \
	} while(0)

/// Allocators
// --Fixed Buffer--
typedef struct Totem_FixedBufferAllocator
{
	void* const start;
	size_t offset;
	size_t capacity;
} FixedBufferAllocator;

FixedBufferAllocator FixedBufferAllocator_Init(void* const memory, size_t capacity);
allocator_t FixedBufferAllocator_GetAllocator(FixedBufferAllocator* const fba);
void FixedBufferAllocator_Reset(FixedBufferAllocator* const fba);

// --Page--
extern allocator_t* const PageAllocator;

#endif // !_TOTEM_H
