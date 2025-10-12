#ifndef CM_ALLOCATOR_C
#define CM_ALLOCATOR_C

#if !defined(CRT_LINKED) && !defined(NO_CRT_LINKED)
  #error You should define macro CRT_LINKED or NO_CRT_LINKED to use this file
#endif

#include <cm_memory.c>
#include <memoryapi.h>

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

#endif // CM_ALLOCATOR_C
