#ifndef CM_ENTRY_H
#define CM_ENTRY_H

#include "cm_os_set.h"

#if !defined(CRT_LINKED) && !defined(NO_CRT_LINKED)
  #error You must define ONLY ONE of CRT_LINKED or NO_CRT_LINKED macro
#elif defined(CRT_LINKED) && defined(NO_CRT_LINKED)
  #error You must define ONLY ONE of CRT_LINKED or NO_CRT_LINKED macro
#endif

#if CM_WINDOWS
  #if !defined(SUB_CONSOLE) && !defined(SUB_WINDOWS)
    #error You must define macro SUB_CONSOLE or SUB_WINDOWS on win32
  #elif defined(SUB_CONSOLE) && defined(SUB_WINDOWS)
    #error You must define ONLY ONE of SUB_CONSOLE or SUB_WINDOWS macro on win32
  #endif
#endif // CM_WINDOWS

#if defined(NO_CRT_LINKED)
  #if (COMPILER_MSVC)
    #pragma comment(linker, "/NODEFAULTLIB")
  #endif // COMPILER_MSVC
  #define RETURN_FROM_MAIN(x) exit((x))

  #ifdef CM_DLL
    #include "cm_dll.h"
  #else
    #include "cm_windows_entry.h"
  #endif // CM_DLL

#elif defined(CRT_LINKED)
  #include <stdio.h>
  #include <stdlib.h>
  #define RETURN_FROM_MAIN(x) return ((x))

  #if (CM_WINDOWS)
    #ifdef SUB_CONSOLE
      #define ENTRY int main(MU int argc, MU char** argv)

    #if (COMPILER_MSVC)
      #pragma comment(linker, "/SUBSYSTEM:CONSOLE")
      // #pragma comment(linker, "/ENTRY:main")
    #endif // COMPILER_MSVC

    #elifdef SUB_WINDOWS
      #pragma warning(push, 0)
      #include <windows.h>
      #pragma warning(pop)
      EXTERN_C IMAGE_DOS_HEADER __ImageBase;
      #define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
      static HINSTANCE g_hInstance = HINST_THISCOMPONENT;
      #define ENTRY int WinMain(MU HINSTANCE _i, MU HINSTANCE _p, MU LPSTR _c, MU int _s)

      #if (COMPILER_MSVC)
        #pragma comment(linker, "/SUBSYSTEM:WINDOWS")
        // #pragma comment(linker, "/ENTRY:WinMain")
      #endif // COMPILER_MSVC

    #endif // SUB_CONSOLE
  #endif // CM_WINDOWS

#endif // NO_CRT_LINKED

#endif //CM_ENTRY_H
