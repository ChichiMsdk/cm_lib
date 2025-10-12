#ifndef CM_OS_SET_H
#define CM_OS_SET_H

#if defined(__clang__)

  #define COMPILER_CLANG 1
  #if defined(_WIN32)
    #define CM_WINDOWS 1
  #elif defined(__gnu_linux__) || defined(__linux__)
    #define OS_LINUX 1
  #elif defined(__APPLE__) && defined(__MACH__)
    #define OS_MAC 1
  #else
    #error This compiler/OS combo is not supported.
  #endif // _WIN32

#elif defined(_MSC_VER)

  #define COMPILER_MSVC 1
  #if defined(_WIN32)
    #define CM_WINDOWS 1
  #else
    #error MSVC compiler is only usable with WIN32.
  #endif // _WIN32

#else
  #error Unknown compiler.
#endif // __clang__

#if defined(__cplusplus)
  #define CPP_LANG 1
#else
  #define C_LANG 1
#endif

#if !defined(COMPILER_MSVC)
  #define COMPILER_MSVC 0
#endif
#if !defined(COMPILER_GCC)
  #define COMPILER_GCC 0
#endif
#if !defined(COMPILER_CLANG)
  #define COMPILER_CLANG 0
#endif
#if !defined(CM_WINDOWS)
  #define CM_WINDOWS 0
#endif
#if !defined(OS_LINUX)
  #define OS_LINUX 0
#endif
#if !defined(OS_MAC)
  #define OS_MAC 0
#endif
#if !defined(CPP_LANG)
  #define CPP_LANG 0
#endif
#if !defined(C_LANG)
  #define C_LANG 0
#endif

#endif // CM_OS_SET_H
