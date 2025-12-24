#ifndef PROC_THREADS_C
#define PROC_THREADS_C

#include "cm_macro_defs.c"

#if (CM_WINDOWS)

#pragma comment (lib, "User32.lib")
#pragma comment (lib, "Shell32.lib")
#define WIN32_NO_BS
#pragma warning(push, 0)
#include <shellapi.h>
#include <windows.h>
#include <tlhelp32.h>
#pragma warning(pop)

void
log_thread(DWORD dw)
{
  char* msg = "Unknown";
  switch (dw)
  {
    /* case WAIT_OBJECT_0 : msg = "The state of the specified object is signaled."; break; */
    case WAIT_TIMEOUT  : msg = "Time-out interval elapsed, and object's state is nonsignaled."; break;
    case WAIT_FAILED   : report_error("WaitForSingleObject", "WAIT_FAILED"); return;
    default: return;
  }
  printf("%s\n", msg);
}

bool
process_create(char *path, char *args, bool wait, u32 *process_code)
{
  BOOL                value     = FALSE;
  BOOL                inherit   = FALSE;
  void*               env       = NULL;
  char*               cwd       = NULL;
  DWORD               flags     = 0;
  DWORD               dw        = 0;
  STARTUPINFO         si        = {.cb = sizeof(si)};
  PROCESS_INFORMATION pi        = {0};
  SECURITY_ATTRIBUTES pa        = {0};
  SECURITY_ATTRIBUTES ta        = {0};

  value = CreateProcess(path, args, &pa, &ta, inherit, flags, env, cwd, &si, &pi);
  if (value == FALSE)
  {
    /* @Note: why would you close anything if it failed ? */
    /*
     * CloseHandle(pi.hProcess);
     * CloseHandle(pi.hThread);
     */
    report_error("CreateProcess", path);
    return false;
  }
  if (wait)
  {
    dw = WaitForSingleObject(pi.hProcess, 10000); // 10 sec
    log_thread(dw);
    dw = WaitForSingleObject(pi.hThread, 10000); // 10 sec
    log_thread(dw);
    value = GetExitCodeProcess(pi.hProcess, (LPDWORD)process_code);
    if (value == FALSE) report_error("GetExitCodeProcess", path);
  }
  CloseHandle(pi.hThread);
  CloseHandle(pi.hProcess);
  return value;
}

#ifndef NOT_SHELL_EXECUTE
bool
shell_execute(char* path, char* args)
{
  HINSTANCE err = ShellExecute(NULL, "open", path, args, NULL, 1);
  if ((INT_PTR) err <= 32)
  {
    return false;
  }
  return true;
}
#endif

typedef bool (*Function_Process_Do)(PROCESSENTRY32, void*);

static bool
process_list_do(Function_Process_Do fn_user, void *data)
{
  HANDLE          snapshot;
  PROCESSENTRY32  entry;

  entry    = (PROCESSENTRY32) { .dwSize = sizeof(PROCESSENTRY32), };
  snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (snapshot == INVALID_HANDLE_VALUE)
    return false;

  /* @FixMe: Should failure be handled this way ? */
  if (!Process32First(snapshot, &entry)) 
  {
    CloseHandle(snapshot);
    return false;
  }
  do {
    if ( !fn_user(entry, data) ) return false;
  } while (Process32Next(snapshot, &entry));
  /* 
   * @Note: Can we not keep the snapshot longer ?
   *       How much memory / what are the implications ?
   */
  CloseHandle(snapshot);
  return true;
}

inline static bool
_print_all_process_names(PROCESSENTRY32 entry, void *data)
{ return printf("%s\n", entry.szExeFile); }

inline static bool process_list_all(void) { return process_list_do(_print_all_process_names, NULL); }

static bool
process_is_running(char* process_name) 
{
  HANDLE          snapshot;
  PROCESSENTRY32  entry;

  entry    = (PROCESSENTRY32) { .dwSize = sizeof(PROCESSENTRY32), };
  snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (snapshot == INVALID_HANDLE_VALUE)
    return false;

  if (!Process32First(snapshot, &entry)) 
  {
    CloseHandle(snapshot);
    return false;
  }

  do {
    if (!strcmp(entry.szExeFile, process_name))
    {
      CloseHandle(snapshot);
      return true;
    }
  } while (Process32Next(snapshot, &entry));
  CloseHandle(snapshot);
  return false;
}

#endif // CM_WINDOWS

#endif // PROC_THREADS_C
