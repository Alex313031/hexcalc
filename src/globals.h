#ifndef HEXCALC_GLOBALS_H_
#define HEXCALC_GLOBALS_H_

#include "framework.h"

// Main client width/height
extern int cxClient;
extern int cyClient;

extern HINSTANCE g_hInstance; // This program instance, everything descends from this

extern HWND mainHwnd; // Our main window handle

extern volatile bool g_running; // True when calculating

#endif // HEXCALC_GLOBALS_H_
