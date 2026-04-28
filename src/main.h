#ifndef HEXCALC_MAIN_H_
#define HEXCALC_MAIN_H_

#include "framework.h"

extern HWND hInputLabel; // Label for the input edit
extern HWND hInputEdit;  // Handle to input edit child window

// Labels for output controls (signed -> unsigned -> float -> hex).
extern HWND hShortLabel;
extern HWND hIntLabel;
extern HWND hLongLabel;
extern HWND hLongLongLabel;
extern HWND hUintLabel;
extern HWND hUint64Label;
extern HWND hFloatLabel;
extern HWND hDoubleLabel;
extern HWND hLongDoubleLabel;
extern HWND hDwordLabel;
extern HWND hHexLabel;

// Output (read-only edit) controls.
extern HWND hShortOutput;
extern HWND hIntOutput;
extern HWND hLongOutput;
extern HWND hLongLongOutput;
extern HWND hUintOutput;
extern HWND hUint64Output;
extern HWND hFloatOutput;
extern HWND hDoubleOutput;
extern HWND hLongDoubleOutput;
extern HWND hDwordOutput;
extern HWND hHexOutput;

// Button handles
extern HWND hConvertButton;
extern HWND hAboutButton;

// Registers the window class
bool RegisterWndClass(HINSTANCE hInstance);

// Initializes window/mainHwnd
bool InitApp();

// Main window procedure
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Closes all windows and cleans up any resources.
void ShutDownApp();

// Shows help
bool LaunchHelp(HWND hWnd);

// About dialog handler
INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

// Creates controls
bool InitControls(HWND hWnd);

// Lays out / resizes all child controls based on the current client area.
void HandleResize(HWND hWnd);

#endif // HEXCALC_MAIN_H_
