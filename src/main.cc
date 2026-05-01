// Entry point for HexCalc

#include "main.h"

#include "converters.h"
#include "globals.h"
#include "resource.h"
#include "utils.h"
#include "version.h"

HWND mainHwnd = nullptr;

HINSTANCE g_hInstance = nullptr;

int cxClient = 0;
int cyClient = 0;

// Whether to open conhost window for debugging.
static constexpr bool debug_console = is_debug;

static const LPCWSTR appTitle    = APP_NAME;
static const LPCWSTR szClassName = MAIN_WNDCLASS;

HWND hInputLabel = nullptr;
HWND hInputEdit  = nullptr;
// Output labels (signed -> unsigned -> float -> hex).
HWND hShortLabel      = nullptr;
HWND hIntLabel        = nullptr;
HWND hLongLabel       = nullptr;
HWND hLongLongLabel   = nullptr;
HWND hUshortLabel     = nullptr;
HWND hUintLabel       = nullptr;
HWND hUint64Label     = nullptr;
HWND hFloatLabel      = nullptr;
HWND hDoubleLabel     = nullptr;
HWND hLongDoubleLabel = nullptr;
HWND hDwordLabel      = nullptr;
HWND hHexLabel        = nullptr;
// Output (read-only edit) controls.
HWND hShortOutput      = nullptr;
HWND hIntOutput        = nullptr;
HWND hLongOutput       = nullptr;
HWND hLongLongOutput   = nullptr;
HWND hUshortOutput     = nullptr;
HWND hUintOutput       = nullptr;
HWND hUint64Output     = nullptr;
HWND hFloatOutput      = nullptr;
HWND hDoubleOutput     = nullptr;
HWND hLongDoubleOutput = nullptr;
HWND hDwordOutput      = nullptr; // Short hex
HWND hHexOutput        = nullptr; // Long hex
// Buttons
HWND hConvertButton = nullptr;
HWND hAboutButton   = nullptr;

// Drives both creation (InitControls) and layout (HandleResize) for every
// label/output pair, so the two stay in lockstep.
struct OutputRow {
  HWND* label_hwnd;
  HWND* output_hwnd;
  int output_id;
  LPCWSTR label_text;
};

static const OutputRow g_output_rows[] = {
    // Signed integers, narrow to wide.
    {&hShortLabel, &hShortOutput, IDC_SHORT, L"Short (16):"},
    {&hIntLabel, &hIntOutput, IDC_INT, L"Int (32):"},
    {&hLongLabel, &hLongOutput, IDC_LONG, L"Long (32):"},
    {&hLongLongLabel, &hLongLongOutput, IDC_LONGLONG, L"Long Long (64):"},
    // Unsigned integers.
    {&hUshortLabel, &hUshortOutput, IDC_USHORT, L"UShort (16):"},
    {&hUintLabel, &hUintOutput, IDC_UINT, L"UInt (32):"},
    {&hUint64Label, &hUint64Output, IDC_UINT64, L"UInt64 (64):"},
    // Floating point.
    {&hFloatLabel, &hFloatOutput, IDC_FLOAT, L"Float (32):"},
    {&hDoubleLabel, &hDoubleOutput, IDC_DOUBLE, L"Double (64):"},
    {&hLongDoubleLabel, &hLongDoubleOutput, IDC_LONGDOUBLE, L"Long Double (80):"},
    // Hex.
    {&hDwordLabel, &hDwordOutput, IDC_DWORD, L"DWORD (32):"},
    {&hHexLabel, &hHexOutput, IDC_HEX, L"QWORD (64):"},
};
static constexpr size_t kNumOutputRows = sizeof(g_output_rows) / sizeof(g_output_rows[0]);

int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPWSTR lpCmdLine,
                      int iCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  g_hInstance = hInstance;
  // Initialize common controls
  INITCOMMONCONTROLSEX icex;
  icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
  icex.dwICC  = ICC_STANDARD_CLASSES;
  InitCommonControlsEx(&icex);

  if (!RegisterWndClass(g_hInstance)) {
    ErrorBox(nullptr, L"RegisterClassEx Error", L"This program requires Windows NT!");
    return 1;
  } else {
    // Set up our logging using mini_logger library.
    const logging::LogDest kLogSink     = debug_console ? logging::LOG_TO_ALL : logging::LOG_NONE;
    static const std::wstring file_name = std::wstring(INTERNAL_NAME);
    static const std::wstring file_extension = L".log";
    const std::wstring kLogFile              = file_name + file_extension;
    logging::LogInitSettings LoggingSettings;
    LoggingSettings.log_sink          = kLogSink;
    LoggingSettings.logfile_name      = kLogFile;
    LoggingSettings.app_name          = appTitle;
    LoggingSettings.show_func_sigs    = false;
    LoggingSettings.show_line_numbers = false;
    LoggingSettings.show_time         = false;
    LoggingSettings.full_prefix_level = LOG_ERROR;
    const bool init_logging           = logging::InitLogging(g_hInstance, LoggingSettings);
    if (init_logging) {
      logging::SetIsDCheck(is_dcheck);
      LOG(INFO) << L"---- Welcome to HexCalc Win32 ----";
      LOG(INFO) << L"Version: " << GetVersionString();
    } else {
      ErrorBox(nullptr, L"Logging Initialization Failure", L"InitLogging failed!");
      return 2;
    }
  }

  // Initialize globals and open main window
  if (!InitApp()) {
    LOG(ERROR) << L"Failed to create main window!";
    return 3;
  }

  HACCEL hAccel = LoadAcceleratorsW(hInstance, MAKEINTRESOURCEW(IDR_MAIN));

  MSG msg;
  while (GetMessageW(&msg, nullptr, 0, 0)) {
    if (!TranslateAcceleratorW(mainHwnd, hAccel, &msg)) {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
    }
  }
  if (hAccel != nullptr) {
    DestroyAcceleratorTable(hAccel);
  }
  return static_cast<int>(msg.wParam);
}

bool RegisterWndClass(HINSTANCE hInstance) {
  if (g_hInstance == nullptr || hInstance != g_hInstance) {
    return false;
  }
  WNDCLASSEXW wndclass;
  wndclass.cbSize        = sizeof(WNDCLASSEX);
  wndclass.style         = 0;
  wndclass.lpfnWndProc   = WindowProc;
  wndclass.cbClsExtra    = 0;
  wndclass.cbWndExtra    = 0;
  wndclass.hInstance     = hInstance;
  wndclass.hIcon         = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_MAIN));
  wndclass.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
  wndclass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_3DFACE + 1);
  wndclass.lpszMenuName  = MAKEINTRESOURCEW(IDR_MAIN);
  wndclass.lpszClassName = szClassName;
  wndclass.hIconSm       = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_SMALL));

  if (!RegisterClassExW(&wndclass)) {
    return false;
  } else {
    return true;
  }
}

bool InitApp() {
  if (g_hInstance == nullptr) {
    return false;
  }
  static constexpr DWORD style =
      WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_SIZEBOX | WS_CLIPCHILDREN;

  // Create main window
  mainHwnd =
      CreateWindowExW(WS_EX_OVERLAPPEDWINDOW, szClassName, appTitle, style, CW_USEDEFAULT,
                      CW_USEDEFAULT, CW_WIDTH, CW_HEIGHT, nullptr, nullptr, g_hInstance, nullptr);

  if (mainHwnd == nullptr) {
    return false;
  }

  ShowWindow(mainHwnd, SW_NORMAL);
  if (!UpdateWindow(mainHwnd)) {
    return false;
  }
  return true;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_CREATE:
      if (mainHwnd == nullptr) {
        mainHwnd = hWnd; // Prevent race condition
      }
      if (!InitControls(hWnd)) {
        return 1;
      } else {
        SetFontAllControls(hWnd);
      }
      break;
    case WM_GETMINMAXINFO: {
      LPMINMAXINFO pMinMaxInfo      = reinterpret_cast<LPMINMAXINFO>(lParam);
      pMinMaxInfo->ptMinTrackSize.x = MINWIDTH;
      pMinMaxInfo->ptMinTrackSize.y = MINHEIGHT;
      pMinMaxInfo->ptMaxTrackSize.x = MAXWIDTH;
      pMinMaxInfo->ptMaxTrackSize.y = MAXHEIGHT;
      break;
    }
    case WM_PAINT: {
      PAINTSTRUCT ps;
      BeginPaint(hWnd, &ps);
      EndPaint(hWnd, &ps);
      break;
    }
    case WM_SIZE:
      // Resize controls as needed
      HandleResize(hWnd);
      break;
    case WM_COMMAND: {
      const int command = LOWORD(wParam);
      switch (command) {
        case IDM_EXIT:
          ShutDownApp();
          break;
        case IDC_ABOUT:
        case IDM_ABOUT:
          PlaySoundW(L"SystemNotification", nullptr, SND_ALIAS | SND_ASYNC);
          DialogBoxW(g_hInstance, MAKEINTRESOURCEW(IDD_ABOUTDLG), hWnd, AboutDlgProc);
          break;
        case IDM_HELP:
          LaunchHelp(hWnd);
          break;
        case IDM_TESTTRAP:
          PlaySoundW(MAKEINTRESOURCEW(SND_ALIAS_SYSTEMEXCLAMATION), nullptr,
                     SND_ALIAS | SND_SYNC | SND_NODEFAULT);
          TestTrap();
          break;
        case IDC_CONVERT:
          ConvertNumbers();
          break;
        default:
          return DefWindowProcW(hWnd, message, wParam, lParam);
      }
    } break;
    case WM_HELP:
      LaunchHelp(hWnd);
      break;
    case WM_CLOSE:
      ShutDownApp();
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    case WM_NCDESTROY:
      mainHwnd = nullptr;
      break;
    default:
      return DefWindowProcW(hWnd, message, wParam, lParam);
  }
  return 0;
}

void ShutDownApp() {
  // De-initialize logging, which closes any console window open
  logging::DeInitLogging(g_hInstance); // Can't log anything more after this
  // WM_DESTROY will call ShutdownAnts() for us - DestroyWindow triggers that
  // path synchronously, so we don't need to touch thread state here.
  DestroyWindow(mainHwnd);
}

bool LaunchHelp(HWND hWnd) {
  if (InfoBox(hWnd, L"Help32", L"No help yet...")) {
    return true;
  }
  return false;
}

INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
  UNREFERENCED_PARAMETER(lParam);
  switch (message) {
    case WM_INITDIALOG:
      // Set icon in titlebar of about dialog
      static const HICON kAboutIcon = LoadIconW(g_hInstance, MAKEINTRESOURCEW(IDI_ABOUT));
      SendMessageW(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)kAboutIcon);
      SendMessageW(hDlg, WM_SETICON, ICON_BIG, (LPARAM)kAboutIcon);
      return TRUE;
    case WM_CLOSE:
      EndDialog(hDlg, TRUE);
      return TRUE;
    case WM_COMMAND:
      if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
        EndDialog(hDlg, LOWORD(wParam));
        return TRUE;
      }
      break;
    default:
      break;
  }
  return FALSE;
}

bool InitControls(HWND hWnd) {
  if (hWnd == nullptr || hWnd != mainHwnd) {
    return false;
  }

  // Create everything at (0,0,0,0); HandleResize positions/sizes them once
  // WM_SIZE fires (which it does before the window is shown).
  hInputLabel = CreateWindowExW(0, WC_STATIC, L"Input:", dwCHILD | dwROW_LABEL, 0, 0, 0, 0, hWnd,
                                (HMENU)IDC_STATIC, g_hInstance, nullptr);
  hInputEdit =
      CreateWindowExW(WS_EX_CLIENTEDGE, WC_EDIT, L"", dwCHILD | ES_AUTOHSCROLL | WS_TABSTOP, 0, 0,
                      0, 0, hWnd, (HMENU)IDC_INPUT, g_hInstance, nullptr);

  for (size_t i = 0; i < kNumOutputRows; ++i) {
    const OutputRow& r = g_output_rows[i];
    *r.label_hwnd  = CreateWindowExW(0, WC_STATIC, r.label_text, dwCHILD | dwROW_LABEL, 0, 0, 0, 0,
                                     hWnd, (HMENU)IDC_STATIC, g_hInstance, nullptr);
    *r.output_hwnd = CreateWindowExW(WS_EX_CLIENTEDGE, WC_EDIT, L"", dwCHILD | dwOUTPUT_EDIT, 0, 0,
                                     0, 0, hWnd, (HMENU)(INT_PTR)r.output_id, g_hInstance, nullptr);
    if (*r.label_hwnd == nullptr || *r.output_hwnd == nullptr) {
      return false;
    }
  }

  hConvertButton = CreateWindowExW(0, WC_BUTTON, L"Convert", dwCHILD | dwBUTTON, 0, 0, 0, 0, hWnd,
                                   (HMENU)IDC_CONVERT, g_hInstance, nullptr);
  hAboutButton   = CreateWindowExW(0, WC_BUTTON, L"About", dwCHILD | dwBUTTON, 0, 0, 0, 0, hWnd,
                                   (HMENU)IDC_ABOUT, g_hInstance, nullptr);

  return hInputLabel && hInputEdit && hConvertButton && hAboutButton;
}

void HandleResize(HWND hWnd) {
  if (hWnd == nullptr || hWnd != mainHwnd || hInputEdit == nullptr) {
    return;
  }

  RECT rcClient;
  if (!GetClientRect(hWnd, &rcClient)) {
    return;
  }
  cxClient = rcClient.right - rcClient.left;
  cyClient = rcClient.bottom - rcClient.top;
  if (cxClient <= 0 || cyClient <= 0) {
    return; // Minimized or zero-sized; nothing to lay out.
  }

  const int pad      = kClientPadding;
  const int intra    = kIntraPadding;
  const int rowH     = kEditControlHeight;
  const int labelW   = kLabelWidth;
  const int outputX  = pad + labelW + intra;
  const int outputW  = (cxClient - pad) - outputX;
  const int safeOutW = outputW > 0 ? outputW : 0;

  // 1 input label + 1 input edit + 2 per output row + 2 buttons.
  const int total = 2 + static_cast<int>(kNumOutputRows) * 2 + 2;
  HDWP hdwp       = BeginDeferWindowPos(total);
  if (hdwp == nullptr) {
    return;
  }
  static constexpr UINT kFlags = SWP_NOZORDER | SWP_NOACTIVATE;

  int yPos = pad;

  // Input row.
  hdwp = DeferWindowPos(hdwp, hInputLabel, nullptr, pad, yPos, labelW, rowH, kFlags);
  hdwp = DeferWindowPos(hdwp, hInputEdit, nullptr, outputX, yPos, safeOutW, rowH, kFlags);
  yPos += rowH + intra * 2; // Slightly larger gap between input and outputs.

  // Output rows.
  for (size_t i = 0; i < kNumOutputRows; ++i) {
    const OutputRow& r = g_output_rows[i];
    hdwp = DeferWindowPos(hdwp, *r.label_hwnd, nullptr, pad, yPos, labelW, rowH, kFlags);
    hdwp = DeferWindowPos(hdwp, *r.output_hwnd, nullptr, outputX, yPos, safeOutW, rowH, kFlags);
    yPos += rowH + intra;
  }

  // Buttons centered both horizontally and vertically in the leftover space
  // between the last output row and the bottom edge (with client padding).
  const int btnAreaTop    = yPos; // Just past the last row.
  const int btnAreaBottom = cyClient - pad;
  const int btnAreaH      = btnAreaBottom - btnAreaTop;
  int btnY                = btnAreaTop + (btnAreaH - kButtonHeight) / 2;
  if (btnY < btnAreaTop) {
    btnY = btnAreaTop; // Tight fit: pin to top of leftover space.
  }
  const int btnsW = kButtonWidth * 2 + intra;
  const int btnX  = (cxClient - btnsW) / 2;
  hdwp = DeferWindowPos(hdwp, hConvertButton, nullptr, btnX, btnY, kButtonWidth, kButtonHeight,
                        kFlags);
  hdwp = DeferWindowPos(hdwp, hAboutButton, nullptr, btnX + kButtonWidth + intra, btnY,
                        kButtonWidth, kButtonHeight, kFlags);

  EndDeferWindowPos(hdwp);
}
