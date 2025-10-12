#ifndef CM_WINDOWS_ENTRY_H
#define CM_WINDOWS_ENTRY_H
#if (CM_WINDOWS)
  #ifdef SUB_CONSOLE
    #define ENTRY void mainCRTStartup(void)

    #if COMPILER_MSVC
      #pragma comment(linker, "/SUBSYSTEM:CONSOLE")
      #pragma comment(linker, "/ENTRY:mainCRTStartup")
    #endif // COMPILER_MSVC

  #elifdef SUB_WINDOWS
    #pragma warning(push, 0)
    #include <windows.h>
    #pragma warning(pop)
    EXTERN_C IMAGE_DOS_HEADER __ImageBase;
    #define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
    static HINSTANCE g_hInstance = HINST_THISCOMPONENT;
    #define ENTRY void WinMainCRTStartup(void)

    #if (COMPILER_MSVC)
      #pragma comment(linker, "/SUBSYSTEM:WINDOWS")
      #pragma comment(linker, "/ENTRY:WinMainCRTStartup")
    #endif // COMPILER_MSVC

  #endif // SUB_CONSOLE
#endif
#endif // CM_WINDOWS_ENTRY_H
