#ifndef CM_IO_C
#define CM_IO_C

#include <cm_macro_defs.c>
#include <cm_error_handling.c>

typedef struct Slice
{
  void  *view;
  u64   size;
}Slice;

#if (CM_WINDOWS)
#include <windows.h>

typedef struct cmFile
{
  char*     path;
  HANDLE    h_file;
  HANDLE    h_map;
  Slice     buffer;
  u64       file_size;
} cmFile;

#elif (OS_LINUX) || (OS_MAC)
#include <unistd.h>

typedef struct cmFile
{
  char  *path;
  FILE  *file;
  void  *map;
  Slice buffer;
  u64   file_size;
} cmFile;

#endif

force_inline CM_CODE
file_mapping_create(cmFile* file, u32 fl_protect, u32 mapping_max_size)
{
  u32     low         = mapping_max_size;
  u32     high        = 0;
  CM_CODE error_value = CM_OK;
  low = ((u32) file->file_size > low) ? low : (u32) file->file_size;
  file->h_map = CreateFileMapping(file->h_file, NULL, fl_protect, high, low, NULL);
  if (!file->h_map)
  {
    error_value = CM_FILE_MAP_FAIL;
  }
  return error_value;
}

force_inline CM_CODE
file_open(char* path, u32 access, u32 share, cmFile* file)
{
  u32     c           = OPEN_EXISTING;
  u32     attr        = FILE_ATTRIBUTE_NORMAL;
  CM_CODE error_value = CM_OK;

  file->h_file = CreateFile(path, access, share, NULL, c, attr, NULL);
  if (!file->h_file || file->h_file == INVALID_HANDLE_VALUE)
  {
    error_value = CM_FILE_OPEN_FAIL;
  }
  if (error_value == CM_OK)
  {
    /* FIXME: Use GetFileSizeEx instead */
    file->path      = path;
    file->file_size = GetFileSize(file->h_file, NULL);
    if (file->file_size == INVALID_FILE_SIZE)
    {
      error_value = CM_INVALID_SIZE;
    }
  }
  return error_value;
}

force_inline CM_CODE
file_create(char* path, u32 access, u32 share, u32 c, cmFile* f)
{
  u32     attr        = FILE_ATTRIBUTE_NORMAL;
  CM_CODE error_value = CM_OK;
  f->h_file = CreateFile(path, access, share, NULL, c, attr, NULL);
  if (!f->h_file || f->h_file == INVALID_HANDLE_VALUE)
  {
    error_value = CM_FILE_OPEN_FAIL;
  }
  if (error_value == CM_OK)
  {
    /* FIXME: Use GetFileSizeEx instead */
    f->path      = path;
    f->file_size = GetFileSize(f->h_file, NULL);
    if (f->file_size == INVALID_FILE_SIZE)
    {
      error_value = CM_INVALID_SIZE;
    }
  }
  return error_value;
}

force_inline CM_CODE 
handle_close(HANDLE h)
{
  CM_CODE error_value = CM_OK;
  if (h != INVALID_HANDLE_VALUE && h != NULL)
  {
    if (CloseHandle(h) == 0)
    {
      error_value = CM_API_FAIL;
    }
  }
  return error_value;
}

static CM_CODE
file_close(cmFile* file)
{
  CM_CODE error_value = CM_OK;
  if (!file)
  {
    error_value = CM_INVALID_DATA;
  }
  if (error_value == CM_OK)
  {
    /* NOTE: Might as well check those, maybe log them somewhere ? */
    error_value       = (UnmapViewOfFile(file->buffer.view)) ? CM_OK : CM_API_FAIL;
    error_value       = handle_close(file->h_map);
    error_value       = handle_close(file->h_file);
    file->buffer.view = NULL;
    file->h_map       = NULL;
    file->h_file      = NULL;
  }
  return error_value;
}

static CM_CODE
file_exist_open_map_sized(
    char*                 path,
    cmFile*               file,
    u32                   max_size,
    u32                   access,
    u32                   fl_protec,
    u32                   desired)
{
  CM_CODE     error_value = CM_OK;
  SYSTEM_INFO sys         = {0};
  DWORD       gran        = 0;

  GetSystemInfo(&sys);
  gran = sys.dwAllocationGranularity;
  error_value = file_open(
      path,
      access,
      FILE_SHARE_READ,
      file);

  if (error_value == CM_OK)
  {
    /* NOTE: We probably want to log this */
    error_value = file_mapping_create(file, fl_protec, max_size);
  }
  if (error_value == CM_OK)
  {
    /* NOTE: we map the entire view of the file in memory */
    file->buffer.view   = MapViewOfFile(file->h_map, desired, 0, 0 * gran, 0);
    error_value         = (file->buffer.view) ? CM_OK : CM_FILE_MAP_FAIL;
    file->buffer.size   = (file->file_size > max_size) ? max_size : file->file_size;
  }
  return error_value;
}

force_inline CM_CODE
file_exist_open_map_ro(char* path, cmFile* file)
{
  u32 max_size = MB(50);
  return file_exist_open_map_sized(
      path,
      file,
      max_size,
      GENERIC_READ,
      PAGE_READONLY,
      FILE_MAP_READ);
}

int64_t
console_write(HANDLE h, void* str, u32 len)
{
  DWORD bytes_written = 0;
  BOOL value = WriteConsole(h,str, len, &bytes_written, NULL);
  if (value == 0) return -1;
  return bytes_written;
}

bool
console_pause(void)
{
  DWORD         eventsRead  = 0;
  DWORD         written     = 0;
  char*         msg         = "Press any key to continue . . .\r\n";
  INPUT_RECORD  inputRecord = {0};

  if (console_write(STDOUT(), msg, (u32) strlen(msg)) == -1)
  {
    report_error("console_write");
    return false;
  }
  while (INFINITE_LOOP)
  {
    if (!ReadConsoleInputA(STDINPUT(), &inputRecord, 1, &eventsRead))
    {
      report_error("ReadConsole");
      return false;
    }
    BOOL key_down = inputRecord.Event.KeyEvent.bKeyDown;
    if (inputRecord.EventType == KEY_EVENT && key_down)
      break;
  }
  return true;
}
#endif // CM_IO_C
