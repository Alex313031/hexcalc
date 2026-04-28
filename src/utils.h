#ifndef HEXCALC_UTILS_H_
#define HEXCALC_UTILS_H_

// clang-format off
#include "framework.h"
#include <logging.h>
// clang-format on

// Color constants
#define RGB_BLACK   RGB(0, 0, 0)
#define RGB_WHITE   RGB(255, 255, 255)
#define RGB_GREY    RGB(128, 128, 128)
#define RGB_RED     RGB(255, 0, 0)
#define RGB_GREEN   RGB(0, 255, 0)
#define RGB_BLUE    RGB(0, 0, 255)
#define RGB_YELLOW  RGB(255, 255, 0)
#define RGB_CYAN    RGB(0, 255, 255)
#define RGB_MAGENTA RGB(255, 0, 255)

// Default window size
inline constexpr INT CW_WIDTH  = 380;
inline constexpr INT CW_HEIGHT = 520;

// Window size constants
inline constexpr INT MINWIDTH  = 280;
inline constexpr INT MINHEIGHT = 480;
inline constexpr INT MAXWIDTH  = 480;
inline constexpr INT MAXHEIGHT = 640;

// Vertical padding to use between items
inline constexpr INT kIntraPadding = 7;

// Padding to use between items and edge of client area
inline constexpr INT kClientPadding = 7;

// Height of edit/static row controls
inline constexpr INT kEditControlHeight = 22;

// Width of the label column (left side of every row)
inline constexpr INT kLabelWidth = 110;

// Button size metrics
inline constexpr INT kButtonWidth = 80;
inline constexpr INT kButtonHeight = 32;

// Window style constants, to not clutter up function declarations
inline constexpr DWORD dwCHILD       = WS_CHILD | WS_VISIBLE;
inline constexpr DWORD dwROW_LABEL   = SS_RIGHT | SS_CENTERIMAGE;
inline constexpr DWORD dwOUTPUT_EDIT = ES_LEFT | ES_AUTOHSCROLL | ES_READONLY | WS_TABSTOP;
inline constexpr DWORD dwBUTTON      = BS_CENTER | BS_VCENTER | WS_TABSTOP;

// Gets the current side by side directory, regardless of where .exe is started from
const std::wstring GetExeDir();

// Test debug trap
void TestTrap();

// Helper functions for MessageBoxW
bool InfoBox(HWND hWnd, const std::wstring& title, const std::wstring& message);

bool WarnBox(HWND hWnd, const std::wstring& title, const std::wstring& message);

bool ErrorBox(HWND hWnd, const std::wstring& title, const std::wstring& message);

// Gets version as human readable wstring.
const std::wstring GetVersionString();

// Gets a known font, with correct sizing
HFONT GetFont(const std::wstring& in_font);

// Sets font for all child windows
bool SetFontAllControls(HWND hParentWnd);

#endif // HEXCALC_UTILS_H_
