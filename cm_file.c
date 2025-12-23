#ifndef CM_FILE_C
#define CM_FILE_C

#include "cm_io.c"
#include "cm_memory.c"

#define file_get_contentsT(fn_file_open, path, buffer, size)\
  do {\
    i32			tries, max_tries;\
    u8*		  memory;\
    u64			page_size;\
    DWORD   read;\
    cmFile  file;\
    CM_CODE error_value;\
    \
    tries     = 0;\
    *size     = 0;\
    *buffer   = NULL;\
    max_tries = 5;\
    page_size = 1024 * 4;\
    heap_alloc_dz(page_size, memory);\
    do \
    {\
      error_value = fn_file_open(path, GENERIC_READ, FILE_SHARE_READ_WRITE_DELETE, &file);\
      if (error_value == CM_OK) break;\
      tries++;\
    } while(tries < max_tries);\
    if (error_value == CM_OK)\
    {\
      do\
      {\
        if (!ReadFile(file.h_file, &memory[(*size)], page_size, &read, NULL))\
        { \
          report("ReadFile");\
          error_value = CM_FILE_READ_FAIL;\
          heap_free_dz(memory); memory = NULL;\
          break;\
        }\
        if (read == 0) break;\
        (*size) += read;\
        if ((*size) > page_size) { page_size *= 2; heap_realloc_dz(page_size, memory, memory); }\
      } while(1);\
      *buffer = memory;\
    }\
    file_close(&file);\
    return error_value;\
  } while(0);

static CM_CODE file_get_contents(char* path, void** buffer, u64 *size)
{ file_get_contentsT(file_open,  path, buffer, size); }

static CM_CODE file_get_contentsW(wchar_t* path, void** buffer, u64 *size)
{ file_get_contentsT(file_openw, path, buffer, size); }

#endif CM_FILE_C
