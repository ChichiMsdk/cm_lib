#ifndef CM_DLL_H
#define CM_DLL_H
#if (CM_WINDOWS)
    #pragma warning(push, 0)
    #include <windows.h>
    #pragma warning(pop)
    EXTERN_C IMAGE_DOS_HEADER __ImageBase;
    #define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
    static HINSTANCE g_hInstance = NULL;
    #define ENTRY BOOL _DllMainCRTStartup(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
#endif
#endif // CM_DLL_H
