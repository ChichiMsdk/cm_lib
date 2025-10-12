#ifndef CM_EVENTS_C
#define CM_EVENTS_C

#include <cm_macro_defs.c>
#include <cm_error_handling.c>

typedef struct cmWindow
{
  char      *title;
  HWND      hwnd;
  HWND      p_hwnd;
  HINSTANCE hinstance;
  i32       w, h;
  i32       x, y;
} cmWindow;

static void
window_create(cmWindow* win, WNDPROC win_proc, bool show)
{
  WNDCLASSEX window_class = {
    .cbSize         = sizeof(window_class),
    .style          = CS_VREDRAW | CS_HREDRAW,
    .lpfnWndProc    = win_proc,
    .hbrBackground  = NULL,
    .hInstance      = win->hinstance,
    .lpszClassName  = "cm_class",
  };
  DWORD ex_style = 0
                 | WS_EX_APPWINDOW
                 | WS_EX_ACCEPTFILES
                 | 0;
  if (RegisterClassEx(&window_class))
  {
    win->hwnd = CreateWindowEx(
        ex_style,
        window_class.lpszClassName,
        "cm_Window",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU,
        win->x, win->y, win->w, win->h, 0, 0,
        win->hinstance,
        0);
  }
  if (!win->hwnd)
  {
    report_error(EXIT_STR, "CreateWindowEx");
    EXIT_FAIL();
  }
  /* SetLayeredWindowAttributes(win->hwnd, 0, 250, LWA_ALPHA); */
  if (show)
  {
    ShowWindow(win->hwnd, SW_SHOWDEFAULT);
    UpdateWindow(win->hwnd);
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
event_dispatch(fn_event fn, void* args, bool *value)
{
  MSG  msg        = {0};
  bool quit       = false;
  bool return_val = false;
  /* PeekMessage(&msg, NULL, 0, 0, PM_REMOVE); */
  GetMessage(&msg, NULL, 0, 0);
  if (msg.message == WM_QUIT) quit = true;
  else
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  if (fn)     return_val = fn(args, msg);
  if (value)  *value     = return_val;

  return quit;
}

#endif // CM_EVENTS_C
