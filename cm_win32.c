#ifndef CM_WIN32_C
#define CM_WIN32_C

#if !defined(CRT_LINKED) && !defined(NO_CRT_LINKED)
  #error You should define macro CRT_LINKED or NO_CRT_LINKED to use this file
#endif

#include <cm_allocator.c>
#include <cm_memory.c>
#include <stdbool.h>
#include <processenv.h>

/* @Warn: Not thread safe ! */
static uint32_t
cwd_get(char* buffer, uint32_t size)
{
  return GetCurrentDirectoryA(size, buffer);
}

/* @Warn: Not thread safe ! */
static bool
cwd_set(char *path)
{
  return SetCurrentDirectoryA(path);
}

static char*
environment_get(void)
{
  return GetEnvironmentStringsA();
}

/* 
 * @Note: `DWORD cmd_line_get_ansi(int, char***)`
 *        Calls CommandLineToArgvW and WideCharToMultiByte
 *        If function fails, the return value is a DWORD from GetLastError().
 */
[[deprecated]] static void
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

typedef struct _Command_Line
{
  char** argv;
  i32    argc;
}_Command_Line;

static bool
command_line_args_ansi(_Command_Line* command_line)
{
  i32       wargvi_size, i;
  char      *arg;
  wchar_t   **wargv;

  wargv = CommandLineToArgvW(GetCommandLineW(), &command_line->argc);
  OR_RETURN(wargv, "CommandLineToArgvW", false);

  wargvi_size = 0;
  for (i = 0; i < command_line->argc;  i++)
  {
    wargvi_size += WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, NULL, 0, NULL, NULL) + 1;
  }

  heap_alloc_dz((sizeof(char *) * (command_line->argc + 1)) + sizeof(char) * wargvi_size, command_line->argv);

  /* @Note: Converting wargv -> argv */
  arg = (char *)&(command_line->argv[command_line->argc + 1]);

  for (i = 0; i < command_line->argc; i++)
  {
    command_line->argv[i] = arg;
    arg += WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, arg, wargvi_size, NULL, NULL) + 1;
  }
  command_line->argv[command_line->argc] = NULL;
  OR_RETURN(command_line->argv, "WideCharToMultiByte", false);
  return true;
}

/* @Note: Returns true if path exists AND is a directory */
static bool
directory_exist(char* path)
{
  u32 dw;

  dw = GetFileAttributesA(path);
  return (dw != INVALID_FILE_ATTRIBUTES) && (dw & FILE_ATTRIBUTE_DIRECTORY);
}

/* 
 * @Warning: This function must be called with an ALLOCATED entire_path !
 *           It will not crash with MSVC but WILL on any other platform !!!
 */
static bool
directory_create_rf(char *entire_path)
{
  i32   len, curr_len;
  char  *end_part;

  if (directory_exist(entire_path))
    return true;

  /* @Note: Remove trailing slashes */
  len = (i32) strlen(entire_path);
  if (len - 1 >= 0 && entire_path[len - 1] == '\\')
    len--;

  /* @Note: Skip drive specifier */
  curr_len = 0;
  if (len >= 3 && entire_path[1] == ':' && entire_path[2] == '\\')
    curr_len = 2;

  /* @Note: We can't create root so skip past any root specifier */
  while (entire_path[curr_len] == '\\')
    curr_len++;

  while (curr_len < len && entire_path[curr_len])
  {
    /* @Note: Get the end of next part to check */
    end_part = cstrchr(entire_path + curr_len, '\\');
    curr_len = (end_part != NULL) ? (i32) (end_part - entire_path) : len;
    /* 
     * @FixMe: We change '\\' to 0 and revert it back later to avoid allocating memory
     *         But if 'entire_path' is in read_only memory this WILL crash..
     */
    entire_path[curr_len] = 0;
    if (!directory_exist(entire_path))
    {
      if (!CreateDirectoryA(entire_path, NULL))
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
