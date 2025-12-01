#ifndef CM_ALLOCATOR_C
#define CM_ALLOCATOR_C

#if !defined(CRT_LINKED) && !defined(NO_CRT_LINKED)
  #error You should define macro CRT_LINKED or NO_CRT_LINKED to use this file
#endif

uint32_t _tls_index = 0;

#include <memoryapi.h>
#include <cm_log.c>

typedef struct Allocator
{
  void*     block;
  size_t    size;
  size_t    capacity;
} Allocator;

Allocator
allocator_create(MU size_t size)
{
  Allocator a = {0};
  return a;
}

static inline void*
halloc_impl(u64 size, char* file, i32 line)
{
	void* mem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
	if (!mem) log_impl("HeapAlloc failed\n", file, line);
	return mem;
}

#define halloc(size) halloc_impl((size), __FILE__, __LINE__)

static inline void*
hfree_impl(void* memory, char* file, i32 line)
{
	BOOL value = HeapFree(GetProcessHeap(), 0, memory);

	if (!value) log_impl("HeapFree failed\n", file, line);
	return memory;
}

#define hfree(memory) hfree_impl((memory), __FILE__, __LINE__)

static inline void*
hrealloc_impl(void* memory, u64 size, char* file, i32 line)
{
	void* new = HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, memory, size);
	if (!new) log_impl("HeapRealloc failed\n", file, line);
	return new;
}

#define hrealloc(memory, size) hrealloc_impl((memory), (size), __FILE__, __LINE__)

#endif // CM_ALLOCATOR_C
