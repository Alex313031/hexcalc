#include "utils.h"

#include "globals.h"
#include "resource.h"

static BOOL CALLBACK SetFontProc(HWND hChild, LPARAM lParam);

const std::wstring GetExeDir() {
  wchar_t exe_path[MAX_PATH];
  HMODULE this_app = GetModuleHandleW(nullptr);
  if (!this_app) {
    return std::wstring();
  }
  DWORD got_path = GetModuleFileNameW(nullptr, exe_path, MAX_PATH);
  if (got_path == 0 || got_path >= MAX_PATH) {
    return std::wstring();
  }

  // Find the last backslash to get the directory
  std::wstring fullPath(exe_path);
  size_t lastSlash = fullPath.find_last_of(L"\\/");
  std::wstring retval;
  if (lastSlash != std::wstring::npos) {
    retval = fullPath.substr(0, lastSlash + 1); // Include trailing slash
  } else {
    retval = fullPath;
  }
  return retval;
}

inline static void __KillInt3Asm() {
#ifdef __MINGW32__
  asm("int3\n\t"
      "ud2");
#else
  __asm int 3 // Execute int3 interrupt
      __asm {
    UD2
  } // Execute 0x0F, 0x0B
#endif // __MINGW32__
}

void TestTrap() {
  __KillInt3Asm();
  return;
}

bool InfoBox(HWND hWnd, const std::wstring& title, const std::wstring& message) {
  HWND hWndTmp;
  if (hWnd == nullptr && mainHwnd != nullptr) {
    hWndTmp = mainHwnd;
  } else {
    hWndTmp = hWnd;
  }
  return (MessageBoxW(hWndTmp, message.c_str(), title.c_str(), MB_OK | MB_ICONINFORMATION) == IDOK);
}

bool WarnBox(HWND hWnd, const std::wstring& title, const std::wstring& message) {
  HWND hWndTmp;
  if (hWnd == nullptr && mainHwnd != nullptr) {
    hWndTmp = mainHwnd;
  } else {
    hWndTmp = hWnd;
  }
  return (MessageBoxW(hWndTmp, message.c_str(), title.c_str(), MB_OK | MB_ICONWARNING) == IDOK);
}

bool ErrorBox(HWND hWnd, const std::wstring& title, const std::wstring& message) {
  HWND hWndTmp;
  if (hWnd == nullptr && mainHwnd != nullptr) {
    hWndTmp = mainHwnd;
  } else {
    hWndTmp = hWnd;
  }
  return (MessageBoxW(hWndTmp, message.c_str(), title.c_str(), MB_OK | MB_ICONERROR) == IDOK);
}

const std::wstring GetVersionString() {
  // VERSION_STRING is a narrow C string literal built by stringize macros,
  // so we can't feed it straight to std::wstring. Build the wide form
  // directly from the same integer macros (single source of truth in
  // version.h) - std::to_wstring keeps it standards-clean across MinGW
  // and MSVC alike.
  return std::to_wstring(MAJOR_VERSION) + L"." + std::to_wstring(MINOR_VERSION) + L"." +
         std::to_wstring(BUILD_VERSION);
}

HFONT GetFont(const std::wstring& in_font) {
  HDC hdc = GetDC(nullptr);
  if (!hdc || in_font.empty()) {
    return nullptr;
  }
  const int height = -MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72);
  ReleaseDC(nullptr, hdc);
  LPCWSTR font_to_use = in_font.c_str();
  HFONT hGetFont =
      CreateFontW(height,                    // Height of each char
                  0,                         // Width of each char, 0 means auto-set
                  0,                         // Angle
                  0,                         // Angle
                  FW_NORMAL,                 // Font weight, can also use int 0-900
                  false,                     // Italic ?
                  false,                     // Underline
                  false,                     // Strikethrough
                  DEFAULT_CHARSET,           // Charset to use TODO update to UTF-8
                  OUT_DEFAULT_PRECIS,        // GDI subpixel precision for chars
                  CLIP_DEFAULT_PRECIS,       // GDI subpixel precision when partially occluded chars
                  DEFAULT_QUALITY,           // Anti-aliasing quality
                  DEFAULT_PITCH | FF_MODERN, // Horizontal pitch and win32 font family
                  font_to_use);              // Name of font
  if (hGetFont == nullptr) {
    LOG(ERROR) << __func__ << L" failed to get " << in_font << L" font!";
    return nullptr;
  }
  return hGetFont;
}

static BOOL CALLBACK SetFontProc(HWND hChild, LPARAM lParam) {
  HFONT hFont = reinterpret_cast<HFONT>(lParam);
  if (!hFont) {
    return false;
  } else {
    SendMessageW(hChild, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
  }
  return true;
}

bool SetFontAllControls(HWND hParentWnd) {
  if (!hParentWnd) {
    return false;
  }
  const HFONT font_to_send = GetFont(L"Tahoma"); // Classic win32 font
  if (font_to_send != nullptr) {
    return EnumChildWindows(hParentWnd, SetFontProc, reinterpret_cast<LPARAM>(font_to_send));
  } else {
    LOG(ERROR) << L"Failed to set font for child windows of HWND " << hParentWnd;
    return false;
  }
}
