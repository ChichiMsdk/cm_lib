#ifndef CM_MACRO_DEFS_H
#define CM_MACRO_DEFS_H

#include "cm_os_set.h"
#include "cm_types.h"

#if !defined(CRT_LINKED) && !defined(NO_CRT_LINKED)
  #error You must define ONE of CRT_LINKED or NO_CRT_LINKED macro
#elif defined(CRT_LINKED) && defined(NO_CRT_LINKED)
  #error You must define ONLY ONE of CRT_LINKED or NO_CRT_LINKED macro
#endif

#if defined(CM_WINDOWS)
  #if !defined(SUB_CONSOLE) && !defined(SUB_WINDOWS)
    #error You must define macro SUB_CONSOLE or SUB_WINDOWS on win32
  #elif defined(SUB_CONSOLE) && defined(SUB_WINDOWS)
    #error You must define ONLY ONE of SUB_CONSOLE or SUB_WINDOWS macro on win32
  #endif
#endif // CM_WINDOWS

#ifdef WIN32_NO_BS

  #define WIN32_LEAN_AND_MEAN
  #define _CRT_SECURE_NO_WARNINGS
  #define NOGDI
  #define NOIME
  #define NOSERVICE
  #define NOPROFILER
  #define NONLS
  #define NOMCX

#endif // WIN32_NO_BS

#include <stdbool.h>

/*
 * #define _deprecated     [[deprecated]]
 * #define _unused         [[maybe_unused]]
 * #define _nodiscard      [[nodiscard]]
 * #define _fallthrough    [[fallthrough]]
 */
#define MU              [[maybe_unused]]
#define ND              [[nodiscard]]
#define FT              [[fallthrough]]
#define global          static
#define atomic          _Atomic
#define fail            false
#define success         true
#define EXPORT					__declspec(dllexport)

#if COMPILER_MSVC || (COMPILER_CLANG && CM_WINDOWS)
  #pragma section(".rdata$", read)
  #define read_only __declspec(allocate(".rdata$"))
#elif (COMPILER_CLANG && OS_LINUX)
  #define read_only __attribute__((section(".rodata")))
#else
/* NOTE: Taken from RADDBG code base in src\base\base_core.h */

// NOTE(rjf): I don't know of a useful way to do this in GCC land.
// __attribute__((section(".rodata"))) looked promising, but it introduces a
// strange warning about malformed section attributes, and it doesn't look
// like writing to that section reliably produces access violations, strangely
// enough. (It does on Clang)
# define read_only
#endif

#if COMPILER_MSVC
  #define force_inline  __forceinline
  #define thread_static __declspec(thread)
#elif COMPILER_CLANG || COMPILER_GCC
  #define force_inline  __attribute__((always_inline))
  #define thread_static __thread
#endif

#define INFINITE_LOOP   1
#define KB(x)           (((uint64_t)(x)) << 10)
#define MB(x)           (((uint64_t)(x)) << 20)
#define GB(x)           (((uint64_t)(x)) << 30)
#define TB(x)           (((uint64_t)(x)) << 40)
#define countof(array)  (sizeof(array) / sizeof(*(array)))
#define COUNTOF(array)  countof((array))
#ifndef offsetof
  #define offsetof(s,m) ((uint64_t)&((((s)*)0)->(m)))
#endif //offsetof
#define OFFSETOF(s,m)   offsetof((s),(m))

#if COMPILER_MSVC
  #define Trap() __debugbreak()
#elif COMPILER_CLANG || COMPILER_GCC
  #define Trap() __builtin_trap()
#else
  #error Unknown trap intrinsic for this compiler.
#endif

#if defined (NO_CRT_LINKED)
  #if CM_WINDOWS
    #pragma warning(push, 0)
    #include <ole2.h>
    EXTERN_C int _fltused             = 0;
    #define exit(value)               ExitProcess((value))
    #pragma warning(pop)
  #endif // CM_WINDOWS

  #define EXIT_SUCCESS              0
  #define EXIT_FAILURE              1
  #define _CRT_STRINGIZE_(x)        #x
  #define _CRT_STRINGIZE(x)         _CRT_STRINGIZE_(x)
  #define _CRT_WIDE_(s)             L ## s
  #define _CRT_WIDE(s)              _CRT_WIDE_(s)
  #define _CRT_CONCATENATE_(a, b)   a ## b
  #define _CRT_CONCATENATE(a, b)    _CRT_CONCATENATE_(a, b)
  #define _CRT_UNPARENTHESIZE_(...) __VA_ARGS__
  #define _CRT_UNPARENTHESIZE(...)  _CRT_UNPARENTHESIZE_ __VA_ARGS__

  #ifdef NDEBUG
    #define assert(exp) ((void)0)
    #define cm_assert(x) assert((x))
    void    _cm_assert(msg, file, line) assert((msg))
  #else
    void    _cm_assert(MU char* msg, MU char* file, MU int line) {Trap();}
    #define cm_assert(x) ((void)((!!(x)) || (_cm_assert((#x), (__FILE__), (__LINE__)), 0)))
  #endif // NDEBUG

#elif defined (CRT_LINKED)
  #ifdef NDEBUG
    #define cm_assert(x) assert((x))
    void    _cm_assert(msg, file, line) assert((msg))
  #else
    void    _cm_assert(MU char* msg, MU char* file, MU int line) {Trap();}
    #define cm_assert(x) ((void)((!!(x)) || (_cm_assert((#x), (__FILE__), (__LINE__)), 0)))
  #endif //NDEBUG

#endif // NO_CRT_LINKED

#if CM_WINDOWS

  #define STDOUT()    GetStdHandle(STD_OUTPUT_HANDLE)
  #define STDERROR()  GetStdHandle(STD_ERROR_HANDLE)
  #define STDINPUT()  GetStdHandle(STD_INPUT_HANDLE)

  #define STDO        GetStdHandle(STD_OUTPUT_HANDLE)
  #define STDE        GetStdHandle(STD_ERROR_HANDLE)
  #define STDI        GetStdHandle(STD_INPUT_HANDLE)
#else
  #define STDOUT()    stdout
  #define STDERROR()  stderr
  #define STDINPUT()  stdin

#endif // CM_WINDOWS

#define EXIT_FAIL()     exit(EXIT_FAILURE)
#define EXIT_SUCCEED()  exit(EXIT_SUCCESS)

#define DO        do{
#define WHILE     }while(0);
#define IF(c)     DO if((c)){
#define HR_IF(c)  DO r = (c); if(r){
#define ENDIF     }WHILE
#define STR_FMT   "%s"

#ifndef EXTERN_C
  #ifdef __cplusplus
    #define EXTERN_C    extern "C"
  #else
    #define EXTERN_C    extern
  #endif
#endif // EXTERN_C

#if COMPILER_MSVC
  #if defined(__SANITIZE_ADDRESS__) && defined(CRT_LINKED)
    #define ASAN_ENABLED 1
    #define NO_ASAN __declspec(no_sanitize_address)
  #else
    #define NO_ASAN
  #endif

#endif // COMPILER_MSVC

#endif //CM_MACRO_DEFS_H
