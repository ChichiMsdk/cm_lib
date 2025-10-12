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
process_create(char* path, char* args, bool wait, u32 *process_code)
{
  BOOL                value     = FALSE;
  BOOL                inherit   = FALSE;
  void*               env       = NULL;
  char*               cwd       = NULL;
  DWORD               flags     = 0;
  STARTUPINFO         si        = {.cb = sizeof(si)};
  PROCESS_INFORMATION pi        = {0};
  SECURITY_ATTRIBUTES pa        = {0};
  SECURITY_ATTRIBUTES ta        = {0};

  value = CreateProcess(path, args, &pa, &ta, inherit, flags, env, cwd, &si, &pi);
  if (value == FALSE)
  {
    /* NOTE: why would you close anything if it failed ?
     * CloseHandle(pi.hProcess);
     * CloseHandle(pi.hThread);
     */
    report_error("CreateProcess", path);
    return false;
  }
  if (wait)
  {
    /* printf("Waiting thread: %ld and process:%ld\n", pi.dwThreadId, pi.dwProcessId); */
    DWORD dw = WaitForSingleObject(pi.hProcess, 10000); // 10 sec
    log_thread(dw);
    dw = WaitForSingleObject(pi.hThread, 10000); // 10 sec
    log_thread(dw);
    value = GetExitCodeProcess(pi.hProcess, (LPDWORD)process_code);
    if (value == FALSE)
      report_error("GetExitCodeProcess", path);
  }
  CloseHandle(pi.hThread);
  CloseHandle(pi.hProcess);
  return value;
}

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

bool
process_list_all(void)
{
  HANDLE          snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  PROCESSENTRY32  entry = { .dwSize = sizeof(PROCESSENTRY32), };

  if (snapshot == INVALID_HANDLE_VALUE)
    return false;

  /* FIXME: Should failure be handled this way ? */
  if (!Process32First(snapshot, &entry)) 
  {
    CloseHandle(snapshot);
    return false;
  }
  do {
    printf("%s\n", entry.szExeFile);
  } while (Process32Next(snapshot, &entry));
  /* NOTE:
   *       Can we not keep the snapshot longer ?
   *       How much memory / what are the implications ?
   */
  CloseHandle(snapshot);
  return true;
}

bool
process_is_running(char* process_name) 
{
  HANDLE          snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  PROCESSENTRY32  entry = { .dwSize = sizeof(PROCESSENTRY32), };

  if (snapshot == INVALID_HANDLE_VALUE)
    return false;

  /* FIXME: Should failure be handled this way ? */
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

  /* NOTE:
   *       Can we not keep the snapshot longer ?
   *       How much memory / what are the implications ?
   */
  CloseHandle(snapshot);
  return false;
}

#endif // CM_WINDOWS

#endif // PROC_THREADS_C
