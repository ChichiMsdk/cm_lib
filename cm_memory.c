#ifndef CM_MEMORY_C
#define CM_MEMORY_C

#include <stdbool.h>
#include <stdint.h>

#include <cm_string.c>
#include <cm_error_handling.c>

#if (CM_WINDOWS)

#pragma warning(push, 0)
#include <windows.h>
#pragma warning(pop)

static bool
page_commit(void* base, uint64_t size)
{
  bool result = VirtualAlloc(base, size, MEM_COMMIT, PAGE_READWRITE);
  return result;
}

static void*
page_reserve(uint64_t size)
{
  return VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
}

static bool
page_release(void* base, uint64_t size)
{
  size = 0;
  return VirtualFree(base, size, MEM_RELEASE);
}

static bool
page_decommit(void* base, uint64_t size)
{
  return VirtualFree(base, size, MEM_DECOMMIT);
}

static void*
page_large_reserve(uint64_t size)
{
  return VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE);
}

char*
heap_error_str(i64 code)
{
  char *msg = "Unknown";
  switch (code)
  {
    case STATUS_NO_MEMORY: 
      {
        msg = "The allocation attempt failed because of a lack of" 
              " available memory or heap corruption.";
        break;
      }
    case STATUS_ACCESS_VIOLATION:
      {
        msg = "The allocation attempt failed because of heap"
              " corruption or improper function parameters.";
        break;
      }
    default: break;
  }
  return msg;
}

#define heap_realloc(_Heap, dwflags, lpMem, dwbytes, _buffer)\
  DO\
    (_buffer) = HeapReAlloc((_Heap), (dwflags), (lpMem), (dwbytes));\
    IF(!(_buffer))\
    console_debug("HeapReAlloc", "Catastrophic memory allocation failure.");\
    EXIT_FAIL();\
    ENDIF\
  WHILE

#define heap_realloc_dz(dwbytes, buffer_in, buffer_out)\
  heap_realloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (buffer_in), (dwbytes), (buffer_out))\

#define heap_alloc_dz(dwbytes, buffer)\
  heap_alloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwbytes, buffer)

/* FIXME: HeapAlloc, HeapRealloc, DO NOT SetLastError().. so this is wrong */
#define heap_alloc(hHeap, dwFlags, dwbytes, _buffer)\
  DO\
    (_buffer) = HeapAlloc((hHeap), (dwFlags), (dwbytes));\
    IF(!(_buffer))\
    console_debug("HeapAlloc", "Catastrophic memory allocation failure.");\
    EXIT_FAIL();\
    ENDIF\
  WHILE

#define heap_free_dz(buffer)\
  heap_free(GetProcessHeap(), 0, (buffer))

#define heap_free(hHeap, dwFlags, lpMem)\
  DO\
    BOOL __value = HeapFree((hHeap), (dwFlags), (lpMem));\
    CHECK_EXIT(__value, "HeapFree", EXIT_FAILURE);\
    WHILE

#define heap_destroy(hHeap)\
  DO\
    BOOL __value = HeapDestroy((hHeap));\
    CHECK_EXIT(__value, "HeapDestroy", EXIT_FAILURE);\
  WHILE

#define heap_create(flOptions, dwInitialSize, dwMaximumSize, buffer)\
  DO\
    (buffer) = HeapCreate((flOptions), (dwInitialSize), (dwMaximumSize));\
    CHECK_EXIT((buffer), "HeapCreate", EXIT_FAILURE);\
  WHILE

static wchar_t*
chars_to_wchars(char* input)
{
  /* NOTE: This makes me wanna vomit */
  u64     len     = strlen(input);
  wchar_t *output = NULL;
  /* FIXME: */
  heap_alloc_dz((len + 1) * sizeof(wchar_t), output);
  for (size_t i = 0; i < len; i++)
  {
    output[i] = (wchar_t)input[i];
  }
  output[len] = L'\0';
  return output;
}
#endif // CM_WINDOWS

#endif // CM_MEMORY_C
