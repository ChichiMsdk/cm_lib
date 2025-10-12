#ifndef CM_WIN32_C
#define CM_WIN32_C

#if !defined(CRT_LINKED) && !defined(NO_CRT_LINKED)
  #error You should define macro CRT_LINKED or NO_CRT_LINKED to use this file
#endif

#include <cm_allocator.c>
#include <stdbool.h>
#include <processenv.h>

/* WARN: Not thread safe ! */
uint32_t
cwd_get(char* buffer, uint32_t size)
{
  return GetCurrentDirectory(size, buffer);
}

/* WARN: Not thread safe ! */
bool
cwd_set(char* path)
{
  BOOL value = SetCurrentDirectory(path);
  if (value == FALSE)
  {
    return false;
  }
  return true;
}

char*
environment_get(void)
{
  return GetEnvironmentStrings();
}

/* 
 * INFO:  `DWORD cmd_line_get_ansi(int, char***)`
 *        Calls CommandLineToArgvW and WideCharToMultiByte
 *        If function fails, the return value is a DWORD from GetLastError().
 */
static void
cmd_line_get_ansi(int* argc, char*** argv)
{
  // Get the command line arguments as wchar_t strings
  wchar_t ** wargv = CommandLineToArgvW(GetCommandLineW(), argc);
  CHECK_EXIT(wargv, "CommandLineToArgvW", EXIT_FAILURE);
  // Count the number of bytes necessary to store the UTF-8 versions of those strings
  int n = 0;
  for (int i = 0;  i < *argc;  i++)
  {
    n += WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, NULL, 0, NULL, NULL) + 1;
  }
  // Allocate the argv[] array + all the UTF-8 strings
  heap_alloc_dz( (*argc + 1) * sizeof(char *) + n, *argv);
  // Convert all wargv[] --> argv[]
  char* arg = (char *)&((*argv)[*argc + 1]);
  for (int i = 0;  i < *argc;  i++)
  {
    (*argv)[i] = arg;
    arg += WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, arg, n, NULL, NULL) + 1;
  }
  (*argv)[*argc] = NULL;
  CHECK_EXIT((*argv), "WideCharToMultiByte", EXIT_FAILURE);
}

/* NOTE: Returns true if path exists AND is a directory */
static bool
directory_exist(char* path)
{
  DWORD dw = GetFileAttributes(path);
  if (dw != INVALID_FILE_ATTRIBUTES)
  { 
    if (dw & FILE_ATTRIBUTE_DIRECTORY)
    {
      return true;
    }
  }
  return false;
}

static bool
directory_create_rf(char* entire_path)
{
  if (directory_exist(entire_path))
  {
    return true;
  }
  /* NOTE: Remove trailing slashes */
  int len = (int) strlen(entire_path);
  if (len - 1 >= 0 && entire_path[len - 1] == '\\')
    len--;

  /* NOTE: Skip drive specifier */
  int curr_len = 0;
  if (len >= 3 && entire_path[1] == ':' && entire_path[2] == '\\')
    curr_len = 2;

  // We can't create root so skip past any root specifier
  while (entire_path[curr_len] == '\\')
    curr_len++;

  while (curr_len < len && entire_path[curr_len])
  {
    /* NOTE: Get the end of next part to check */
    char* end_part = cstrchr(entire_path + curr_len, '\\');
    if (end_part != NULL)
      curr_len = (int)(end_part - entire_path);
    else
      curr_len = len;

    /* FIXME: 
     *        We change '\\' to 0 and revert it back later to avoid allocating memory
     *        But if 'entire_path' is in read_only memory this WILL crash..
     */
    entire_path[curr_len] = 0;
    if (!directory_exist(entire_path))
    {
      if (!CreateDirectory(entire_path, NULL))
      {
        report_error("CreateDirectory", entire_path);
        return false;
      }
    }
    entire_path[curr_len] = '\\';
    curr_len++;
  }
  return true;
}
#endif // CM_WIN32_C
