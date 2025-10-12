#ifndef CM_ERROR_HANDLING_C
#define CM_ERROR_HANDLING_C

#if CM_WINDOWS
#pragma warning(push, 0)
#include <windows.h>
#pragma warning(pop)
#endif

#include <cm_string.c>

/*
 *  Features: 
 *            [_] Log to specific output (file, console, etc...)
 *            [_] Specify callback functions
 *            [_] Platform agnostic
 */
#define EXIT_STR "Program will exit."
#define CFLR_LEN 5024
global  char  g_file_line_msg[CFLR_LEN];
global  S8    g_debug_info = {.str = g_file_line_msg, .len = CFLR_LEN};

typedef enum CM_CODE {
  CM_OK                 = 0,
  CM_API_FAIL           = 1,
  CM_SIZE_TOO_BIG       = 6,
  CM_INVALID_SIZE       = 7,
  CM_OVERFLOW           = 8,

  CM_INVALID_DATA       = 10,

  CM_FILE_OPEN_FAIL     = 20,
  CM_DIR_OPEN_FAIL      = 21,
  CM_FILE_MAP_FAIL      = 24,
  CM_FILE_MAP_TOO_SMALL = 25,

  CM_FONT_ERROR         = 30,

  CM_HEAP_CREATE_FAIL   = 50,
  CM_HEAP_ALLOC_FAIL    = 51,

  CM_CODE_MAX
} CM_CODE;

typedef enum ErrorType
{
  ET_HRESULT       = 0x0,
  ET_INT           = 0x1,
  ET_WIN_BOOL      = 0x2,
  ET_WIN_HANDLE    = 0x3,
  ET_D3D12         = 0x4,
  ET_CM_CODE       = 0x5,
  ET_UNKNOWN       = 0x6,

  MAX_ERROR_TYPE
} ErrorType;

/*
 * NOTE:
 */
typedef struct ErrorCode
{
  int       line;
  ErrorType type;
  char*     file_name;
  char*     caller_fn_name;
  char*     stack_fn_name;
  union {
    int     int_r;
    BOOL    winbool_r;
    HANDLE  winhandle_r;
    CM_CODE cm_r;
    HRESULT winh_r;
  };
  int       handled;
  int       padding;
} ErrorCode;

static inline ErrorCode
cm_error_init(char* s_fn, char* c_fn, int line, char* file, ErrorType type)
{
  ErrorCode code  = {
    .line           = line,
    .file_name      = file,
    .stack_fn_name  = s_fn,
    .caller_fn_name = c_fn,
    .type           = type,
  };
  switch (type)
  {
    case ET_HRESULT     : code.winh_r      = S_OK; break;
    case ET_INT         : code.int_r       = 1;    break;
    case ET_WIN_BOOL    : code.winbool_r   = TRUE; break;
    case ET_WIN_HANDLE  : code.winhandle_r = NULL; break;
    case ET_D3D12       : code.winh_r      = S_OK; break;
    case ET_CM_CODE     : code.cm_r        = CM_OK;break;

    case ET_UNKNOWN:
    case MAX_ERROR_TYPE:
    default: code.type = ET_UNKNOWN;
  }
  return code;
}

#define R(res, t) ErrorCode (res) = cm_error_init(__FUNCTION__, NULL, __LINE__, __FILE__, (t));

static char*
cm_code_get_string(CM_CODE code)
{
  char* str = "Unknown code";
  switch (code)
  {
    case CM_OK                 : str = "CM_OK"; break;
    case CM_API_FAIL           : str = "CM_API_FAIL"; break;
    case CM_SIZE_TOO_BIG       : str = "CM_SIZE_TOO_BIG"; break;
    case CM_INVALID_SIZE       : str = "CM_INVALID_SIZE"; break;
    case CM_INVALID_DATA       : str = "CM_INVALID_DATA"; break;
    case CM_FILE_OPEN_FAIL     : str = "CM_FILE_OPEN_FAIL"; break;
    case CM_DIR_OPEN_FAIL      : str = "CM_DIR_OPEN_FAIL"; break;
    case CM_FILE_MAP_FAIL      : str = "CM_FILE_MAP_FAIL"; break;
    case CM_FILE_MAP_TOO_SMALL : str = "CM_FILE_MAP_TOO_SMALL"; break;
    case CM_FONT_ERROR         : str = "CM_FONT_ERROR"; break;
    case CM_HEAP_CREATE_FAIL   : str = "CM_HEAP_CREATE_FAIL"; break;
    case CM_HEAP_ALLOC_FAIL    : str = "CM_HEAP_ALLOC_FAIL"; break;
    case CM_OVERFLOW           : str = "CM_OVERFLOW"; break;
    case CM_CODE_MAX:
    default                    : break;
  }
  return str;
}

static void
debug_info_gather(S8 buffer, char* lvl, char* file, int line, char* fn_call, char* fn_ctx)
{
  ZeroMemory(buffer.str, buffer.len);
  int   len = (int)buffer.len - 1;
  char* fmt = "%s: %s:%d: in function `%s` - Thread %d\n\t From `%s`";
  wnsprintf(buffer.str, len, fmt, lvl, file, line, fn_call, GetCurrentThreadId(), fn_ctx);
}

static char*
get_last_error_str(DWORD code)
{
  char*   msg_buf   = NULL;
  LPCVOID source    = NULL;
  DWORD   lang      = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
  DWORD   dw_flags = 0
      | FORMAT_MESSAGE_ALLOCATE_BUFFER
      | FORMAT_MESSAGE_FROM_SYSTEM
      | FORMAT_MESSAGE_IGNORE_INSERTS
      | 0;
  if (FormatMessage(dw_flags, source, code, lang, (LPSTR)&msg_buf, 0, NULL) == 0)
  {
    /* TODO: Handle the error when 128K bytes and GetLastError() returns ERROR_MORE_DATA */
    DWORD error = GetLastError();
    if (FormatMessage(dw_flags, NULL, error, lang, (LPSTR)&msg_buf, 0, NULL) == 0)
    {
      printf("FormatMessage failed twice, first error: (%ld)\n"
             "Second error: (%ld).\n", error, GetLastError());
      return NULL;
    }
  }
  return msg_buf;
}

static char*
get_full_error_msg(char* str, DWORD err)
{
  char* error_str    = get_last_error_str(err);
  char* final_output = NULL;
  if (error_str)
  {
    /* NOTE: For now we use a global but that should change */
    /* NOTE: Use String ? */

    /* FIXME: Alloc */
    final_output = HeapAlloc(
        GetProcessHeap(),
        HEAP_ZERO_MEMORY,
        sizeof(char) * (c_strlen(error_str) + c_strlen(g_file_line_msg) + c_strlen(str) + 6 + 1));

    if (!str || !str[0]) wsprintf((char*)final_output, "%s\n\t %s\n", g_file_line_msg, error_str);
    else wsprintf((char*)final_output, "%s\n\t %s: %s\n", g_file_line_msg, str, error_str);
    LocalFree(error_str);
  }
  return final_output;
}

static void
show_error_msg_box_v(DWORD err, ...)
{
  char*   final     = NULL;
  char*   error_str = get_last_error_str(err);
  if (error_str)
  {
    u32     str_count = 0;
    u64     args_size = 0;
    char    *str;
    va_list args;
    va_start(args, err);
    while ((str = va_arg(args, char*)))
    {
      str_count++;
      args_size += strlen(str);
    }
    str_count = (str_count == 0) ? 1 : str_count;
    va_end(args);
    /* FIXME: Alloc */
    u64 err_size  = c_strlen(error_str);
    u64 flm_size  = c_strlen(g_file_line_msg);
    u64 full_size = err_size + flm_size + args_size + 7 + (str_count * 2) + 1;
    final = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(char) * full_size);

    args_size = wsprintf(final, "%s -> %s", g_file_line_msg, error_str);

    va_start(args, err);
    while ((str = va_arg(args, char*)))
    {
      args_size += wsprintf(final + args_size, "%s ", str);
    }
    va_end(args);

    LocalFree(error_str);
    message_box(final);

    /* FIXME: Alloc */
    HeapFree(GetProcessHeap(), 0, final);
  }
}

static void
show_error_msg_console_v(DWORD err, ...)
{
  char*   final     = NULL;
  char*   error_str = get_last_error_str(err);
  if (error_str)
  {
    u32     str_count = 0;
    u64     args_size = 0;
    char    *str;
    va_list args;
    va_start(args, err);
    while ((str = va_arg(args, char*)))
    {
      str_count++;
      args_size += strlen(str);
    }
    str_count = (str_count == 0) ? 1 : str_count;
    va_end(args);
    /* FIXME: Alloc */
    u64 err_size  = c_strlen(error_str);
    u64 flm_size  = c_strlen(g_file_line_msg);
    u64 full_size = err_size + flm_size + args_size + 7 + (str_count * 2) + 1;
    final = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(char) * full_size);

    args_size = wsprintf(final, "%s -> %s", g_file_line_msg, error_str);

    va_start(args, err);
    while ((str = va_arg(args, char*)))
    {
      args_size += wsprintf(final + args_size, "%s ", str);
    }
    va_end(args);

    LocalFree(error_str);
    printf("%s", final);

    /* FIXME: Alloc */
    HeapFree(GetProcessHeap(), 0, final);
  }
}

static inline void
show_error_msg_console(char* str, DWORD err)
{
  char* final_output = get_full_error_msg(str, err);
  if (final_output)
  {
    printf("%s", final_output);
    /* FIXME: Alloc */
    HeapFree(GetProcessHeap(), 0, final_output);
  }
}

static inline void
show_error_msg_box(char* str, DWORD err)
{
  char* final_output = get_full_error_msg(str, err);
  if (final_output)
  {
		/* FIXME: Always check the value ? */
    i32 error = message_box(final_output);
		if (!error)
		{
			debug_info_gather(g_debug_info, "[ERROR]", __FILE__, __LINE__, "message_box", __FUNCTION__);
			show_error_msg_console_v(GetLastError(), NULL);
		}
    /* FIXME: Alloc */
    HeapFree(GetProcessHeap(), 0, final_output);
  }
}

#define CM_OUT_BOX      0b001
#define CM_OUT_CONSOLE  0b010
#define CM_OUT_HANDLE   0b100
#define CM_OUT_ALL      0b111

#define box_debug(fn_call, ...)\
  DO\
  debug_info_gather(g_debug_info, "[ERROR]", __FILE__, __LINE__, (fn_call), __FUNCTION__);\
  log_debug(CM_OUT_CONSOLE, g_file_line_msg __VA_OPT__(,) __VA_ARGS__, NULL);\
  WHILE

#define console_debug_v(fn_call, fmt, ...)\
  DO\
  debug_info_gather(g_debug_info, "[ERROR]", __FILE__, __LINE__, (fn_call), __FUNCTION__);\
  log_debug_v(CM_OUT_CONSOLE, g_file_line_msg, (fmt) __VA_OPT__(,) __VA_ARGS__, NULL);\
  WHILE

#define console_debug(fn_call, ...)\
  DO\
  debug_info_gather(g_debug_info, "[ERROR]", __FILE__, __LINE__, (fn_call), __FUNCTION__);\
  log_debug(CM_OUT_CONSOLE, g_file_line_msg __VA_OPT__(,) __VA_ARGS__, NULL);\
  WHILE

#define report_error_box(fn_call, ...) \
  DO\
    debug_info_gather(g_debug_info, "[WIN32]", __FILE__, __LINE__, (fn_call), __FUNCTION__);\
    show_error_msg_box_v(GetLastError(), ## __VA_ARGS__, NULL);\
  WHILE

#define report_error_go(fn_call, label, ...) \
  DO\
    debug_info_gather(g_debug_info, "[WIN32]", __FILE__, __LINE__, (fn_call), __FUNCTION__);\
    show_error_msg_console_v(GetLastError(), ## __VA_ARGS__, NULL);\
    goto (label);\
  WHILE

#define report_error(fn_call, ...) \
  DO\
    debug_info_gather(g_debug_info, "[WIN32]", __FILE__, __LINE__, (fn_call), __FUNCTION__);\
    show_error_msg_console_v(GetLastError(), ## __VA_ARGS__, NULL);\
  WHILE

#define report_console(x, fn_call) \
  DO\
    debug_info_gather(g_debug_info, "[ERROR]", __FILE__, __LINE__, (fn_call), __FUNCTION__);\
    show_error_msg_console((x), GetLastError());\
  WHILE

#define report_box(x, fn_call) \
  DO\
    debug_info_gather(g_debug_info, "[ERROR]", __FILE__, __LINE__, (fn_call), __FUNCTION__);\
    show_error_msg_box((x), GetLastError());\
  WHILE

#ifndef report_all
#define report_all(x, fn)\
  DO\
    DWORD __code = GetLastError();\
    debug_info_gather(g_debug_info, "[ERROR]",__FILE__, __LINE__, (fn), __FUNCTION__);\
    show_error_msg_console((x), __code);\
    show_error_msg_box((x), __code);\
  WHILE
#endif // report_all

#ifndef CHECK_EXIT
  #define CHECK_EXIT(cond, str, exit_nb)\
    DO\
      if (!(cond)) {\
        report_all(EXIT_STR, (str));\
        exit((exit_nb));\
      }\
    WHILE
#endif // CHECK_EXIT

static void
log_debug_v(int output, char *buffer, char *fmt, ...)
{
  u64     args_size = 0;
  u32     str_count = 0;
  char*   final;
  va_list args;

  u64 total_size = strlen(buffer) + 1024 * 15;
  final = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(char) * total_size);
  if (!final) {printf("HeapAlloc failed\n"); exit(1);};

  va_start(args, fmt);
  args_size = wsprintf((char*) final, "%s ", buffer);
  args_size = wvnsprintf(final + args_size, (int)total_size, fmt, args);
  if (args_size >= total_size) 
  {
	  report_error("wvnsprintf", "Size too big\n");
	  exit(1);
  };

  if (output & CM_OUT_CONSOLE) printf("%s\n", final);
  if (output & CM_OUT_BOX) message_box(final);

  BOOL value = HeapFree(GetProcessHeap(), 0, final);
  if (!value){printf("HeapFree failed\n"); exit(1);};
}

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
#endif // CM_ERROR_HANDLING_C
