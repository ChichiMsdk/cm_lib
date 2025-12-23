#ifndef CM_EVENTS_C
#define CM_EVENTS_C

#include <cm_macro_defs.c>
#include <cm_error_handling.c>

typedef struct cmWindow
{
  char      *title;
  HWND      hwnd, p_hwnd;
  HINSTANCE hinstance;
  i32       w, h, x, y;
} Window, cmWindow;

static void
window_create(Window *w, WNDPROC win_proc, bool show)
{
  char       *cname, *wname;
  DWORD      ex_style, style;
  WNDCLASSEX window_class;

  cname                      = "cm_class";
  wname                      = "cm_Window";
  style                      = WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU;
  w->hwnd                    = NULL;
  ex_style                   = WS_EX_APPWINDOW | WS_EX_ACCEPTFILES;
  window_class.cbSize        = sizeof(window_class);
  window_class.style         = CS_VREDRAW | CS_HREDRAW;
  window_class.lpfnWndProc   = win_proc;
  window_class.hbrBackground = NULL;
  window_class.hInstance     = w->hinstance;
  window_class.lpszClassName = cname;
  if (RegisterClassEx(&window_class))
  {
    w->hwnd = CreateWindowEx(ex_style, cname, wname, style, w->x, w->y, w->w, w->h, 0, 0, w->hinstance, 0);
  }
  if (!w->hwnd)
  {
    report_error(EXIT_STR, "CreateWindowEx");
    EXIT_FAIL();
  }
  /* SetLayeredWindowAttributes(w->hwnd, 0, 250, LWA_ALPHA); */
  if (show)
  {
    ShowWindow(w->hwnd, SW_SHOWDEFAULT);
    UpdateWindow(w->hwnd);
  }
}

typedef bool (* fn_event)(void*, MSG);

/* 
 * NOTE:
 *       `fn_event` will be called with args and MSG as param after 
 *       TranslateMessage AND DispatchMessage
 *       `value` is a valid pointer that represents `fn_event`'s return value
 *       This function is meant to be called inside a loop
 */
static bool
event_dispatch(fn_event fn, void *args, bool *value)
{
  MSG  msg;
  bool quit, return_val;

  quit       = false;
  return_val = false;
  /* PeekMessage(&msg, NULL, 0, 0, PM_REMOVE); */
  GetMessage(&msg, NULL, 0, 0);
  if (msg.message == WM_QUIT) quit = true;
  else
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  if (fn)     { return_val = fn(args, msg); }
  if (value)  { *value     = return_val; }
  return quit;
}

#endif // CM_EVENTS_C
