#ifndef CM_TIMER_C
#define CM_TIMER_C

#error don't include this file

#include <cm_macro_defs.c>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define exp7           10000000i64     //1E+7     //C-file part
#define exp9         1000000000i64     //1E+9
#define w2ux 116444736000000000i64     //1.jan1601 to 1.jan1970

size_t clock_gettime(struct timespec *spec);

typedef enum e_unit_t
{
  e_second	= 1,
  e_milli_s	= 1000,
  e_micro_s	= 1000 * 1000,
  e_nano_s	= 1000 * 1000 * 1000,
} e_unit_t;

typedef struct Unit_t	{e_unit_t unit;} Unit_t; 

#define SECOND	((Unit_t){e_second})
#define MILLI_S	((Unit_t){e_milli_s})
#define MICRO_S	((Unit_t){e_micro_s})
#define NANO_S	((Unit_t){e_nano_s})

double g_freq = 0.0;
__int64 g_start_time = 0;

char*
get_time_str(Unit_t unit)
{
  char* unit_str = "Unknown unit";
  switch (unit.unit)
  {
    case e_second: { unit_str = "s"; } break;
    case e_milli_s: { unit_str = "ms"; } break;
    case e_micro_s: { unit_str = "us"; } break;
    case e_nano_s: { unit_str = "ns"; } break;
    default: {} break;
  }
  return unit_str;
} 

void
init_counter(void)
{
  LARGE_INTEGER li;
  if(!QueryPerformanceFrequency(&li))
  {
    printf("QueryPerformanceFrequency failed!\n");
  }

  g_freq = (double) (li.QuadPart);

  if (g_freq <= 0)
  {
    printf("g_freq is %f considerd 0.0f/n", g_freq);
  }
  QueryPerformanceCounter(&li);
  g_start_time = li.QuadPart;
}

double
get_counter(Unit_t unit)
{
  LARGE_INTEGER li;
  QueryPerformanceCounter(&li);
  return (double) (li.QuadPart - g_start_time) / (g_freq / unit.unit);
}

// stackoverflow don't recall the link
void
unix_time(struct timespec *spec)
{
  __int64 wintime;
  GetSystemTimeAsFileTime((FILETIME*)&wintime); 
  wintime -=w2ux;
  spec->tv_sec    = wintime / exp7;                 
  spec->tv_nsec   = wintime % exp7 *100;
}

size_t
clock_gettime(struct timespec *spec)
{
  static  struct timespec startspec;
  static double ticks2nano;
  static __int64 startticks, tps =0;    __int64 tmp, curticks;

  QueryPerformanceFrequency((LARGE_INTEGER*)&tmp); //some strange system can
  if (tps !=tmp)
  {
    tps =tmp; //init ~~ONCE         //possibly change freq ?
    QueryPerformanceCounter((LARGE_INTEGER*)&startticks);
    unix_time(&startspec); 
    ticks2nano =(double)exp9 / tps;
  }
  QueryPerformanceCounter((LARGE_INTEGER*)&curticks);
  curticks -=startticks;
  spec->tv_sec    = startspec.tv_sec + (curticks / tps);
  spec->tv_nsec   = startspec.tv_nsec + (double)(curticks % tps) * ticks2nano;
  if (!(spec->tv_nsec < exp9)) 
  {
    spec->tv_sec++;
    spec->tv_nsec -=exp9;
  }
  return spec->tv_nsec;
}
#endif // CM_TIMER_C
