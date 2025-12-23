#ifndef CM_ERRORS_C
#define CM_ERRORS_C

/* NOTE: Replace entirely the very bad cm_error_handling.c */

#include "cm_log.c"

static void
log_debug(int output, char* buffer, ...)
{
  u64     args_size = 0;
  u32     str_count = 0;
  char*   str;
  char*   final;
  va_list args;
  va_start(args, buffer);
  while ((str = va_arg(args, char*)))
  {
    str_count++;
    args_size += strlen(str);
  }
  va_end(args);
  str_count = (str_count > 0) ? str_count : 1;
  u64 total_size = args_size + strlen(buffer) + 1 + str_count + 1;

  final = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(char) * total_size);
  if (!final) {printf("HeapAlloc failed\n"); exit(1);};
  va_start(args, buffer);

  args_size = wsprintf((char*) final, "%s ", buffer);
  while ((str = va_arg(args, char*)))
  {
    args_size += wsprintf(final + args_size, "%s ", str);
  }
  va_end(args);

  if (output & CM_OUT_CONSOLE) printf("%s\n", final);
  if (output & CM_OUT_BOX) message_box(final);

  BOOL value = HeapFree(GetProcessHeap(), 0, final);
  if (!value){printf("HeapFree failed\n"); exit(1);};
}


#endif // CM_ERRORS_C
