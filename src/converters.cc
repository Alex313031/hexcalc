#include "converters.h"

#include <cfloat>  // FLT_MAX, DBL_MAX
#include <climits> // INT_MAX/MIN, etc.
#include <cmath>   // std::isfinite
#include <cstdlib> // wcstold, wcstoull
#include <cwchar>  // swprintf

#include "globals.h"
#include "main.h"
#include "resource.h"
#include "utils.h"

short ConvertToShort(const long double in) {
  return static_cast<short>(in);
}
int ConvertToInt(const long double in) {
  return static_cast<int>(in);
}
long ConvertToLong(const long double in) {
  return static_cast<long>(in);
}
long long ConvertToLongLong(const long double in) {
  return static_cast<long long>(in);
}
unsigned short ConvertToUshort(const long double in) {
  return static_cast<unsigned short>(in);
}
unsigned int ConvertToUint(const long double in) {
  return static_cast<unsigned int>(in);
}
unsigned long long ConvertToUint64(const long double in) {
  return static_cast<unsigned long long>(in);
}
float ConvertToFloat(const long double in) {
  return static_cast<float>(in);
}
double ConvertToDouble(const long double in) {
  return static_cast<double>(in);
}
long double ConvertToLongDouble(const long double in) {
  return in;
}
unsigned int ConvertToDword(const long double in) {
  return static_cast<unsigned int>(in);
}
unsigned long long ConvertToHex64(const long double in) {
  return static_cast<unsigned long long>(in);
}

namespace {
  constexpr size_t kBufSize = 64;

  inline bool IsHexDigit(wchar_t c) {
    return (c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'f') || (c >= L'A' && c <= L'F');
  }
  inline bool IsDecDigit(wchar_t c) {
    return c >= L'0' && c <= L'9';
  }

  // Strict grammar:
  //   <number>  ::= '-'? ( <hex> | <decimal> )
  //   <hex>     ::= '0' ('x'|'X') <hexdigit>+
  //   <decimal> ::= <digit>+ ('.' <digit>*)? | '.' <digit>+
  // No whitespace, no '+', no scientific notation, at most one '.'.
  bool IsValidInputFormat(const wchar_t* s) {
    if (!s || !*s) {
      return false;
    }
    if (*s == L'-') {
      ++s;
      if (!*s) {
        return false; // "-" alone
      }
    }
    // Hex path.
    if (s[0] == L'0' && (s[1] == L'x' || s[1] == L'X')) {
      s += 2;
      if (!*s) {
        return false; // "0x" with no digits
      }
      while (*s) {
        if (!IsHexDigit(*s)) {
          return false;
        }
        ++s;
      }
      return true;
    }
    // Decimal path.
    bool seen_digit = false;
    bool seen_dot   = false;
    while (*s) {
      if (IsDecDigit(*s)) {
        seen_digit = true;
      } else if (*s == L'.') {
        if (seen_dot) {
          return false; // second '.'
        }
        seen_dot = true;
      } else {
        return false; // any other char
      }
      ++s;
    }
    return seen_digit;
  }

  // Parse a strict-format number into a long double.  Caller-trusted: returns
  // false only when the input fails IsValidInputFormat; any string that passes
  // validation is guaranteed to convert via wcstoull/wcstold.
  bool ParseInput(const wchar_t* s, long double* out) {
    if (!IsValidInputFormat(s)) {
      return false;
    }
    bool neg         = false;
    const wchar_t* p = s;
    if (*p == L'-') {
      neg = true;
      ++p;
    }
    if (p[0] == L'0' && (p[1] == L'x' || p[1] == L'X')) {
      wchar_t* end               = nullptr;
      const unsigned long long u = wcstoull(p, &end, 16);
      *out                       = neg ? -static_cast<long double>(u) : static_cast<long double>(u);
      return true;
    }
    wchar_t* end        = nullptr;
    const long double v = wcstold(s, &end);
    *out                = v;
    return true;
  }

  void SetAllOutputs(LPCWSTR text) {
    SetWindowTextW(hShortOutput, text);
    SetWindowTextW(hIntOutput, text);
    SetWindowTextW(hLongOutput, text);
    SetWindowTextW(hLongLongOutput, text);
    SetWindowTextW(hUshortOutput, text);
    SetWindowTextW(hUintOutput, text);
    SetWindowTextW(hUint64Output, text);
    SetWindowTextW(hFloatOutput, text);
    SetWindowTextW(hDoubleOutput, text);
    SetWindowTextW(hLongDoubleOutput, text);
    SetWindowTextW(hDwordOutput, text);
    SetWindowTextW(hHexOutput, text);
  }

  // Per-type formatters: each returns either the formatted typed value, or
  // the matching <climits>/<cfloat> limit name when 'r' would saturate.

  std::wstring FmtShort(long double r) {
    if (r > static_cast<long double>(SHRT_MAX)) {
      return L"SHRT_MAX";
    }
    if (r < static_cast<long double>(SHRT_MIN)) {
      return L"SHRT_MIN";
    }
    wchar_t b[kBufSize];
    swprintf(b, kBufSize, L"%hd", ConvertToShort(r));
    return b;
  }
  std::wstring FmtInt(long double r) {
    if (r > static_cast<long double>(INT_MAX)) {
      return L"INT_MAX";
    }
    if (r < static_cast<long double>(INT_MIN)) {
      return L"INT_MIN";
    }
    wchar_t b[kBufSize];
    swprintf(b, kBufSize, L"%d", ConvertToInt(r));
    return b;
  }
  std::wstring FmtLong(long double r) {
    if (r > static_cast<long double>(LONG_MAX)) {
      return L"LONG_MAX";
    }
    if (r < static_cast<long double>(LONG_MIN)) {
      return L"LONG_MIN";
    }
    wchar_t b[kBufSize];
    swprintf(b, kBufSize, L"%ld", ConvertToLong(r));
    return b;
  }
  std::wstring FmtLongLong(long double r) {
    if (r > static_cast<long double>(LLONG_MAX)) {
      return L"LLONG_MAX";
    }
    if (r < static_cast<long double>(LLONG_MIN)) {
      return L"LLONG_MIN";
    }
    wchar_t b[kBufSize];
    swprintf(b, kBufSize, L"%lld", ConvertToLongLong(r));
    return b;
  }
  std::wstring FmtUshort(long double r) {
    if (r > static_cast<long double>(USHRT_MAX)) {
      return L"USHRT_MAX";
    }
    if (r < 0) {
      return L"0";
    }
    wchar_t b[kBufSize];
    swprintf(b, kBufSize, L"%hu", ConvertToUshort(r));
    return b;
  }
  std::wstring FmtUint(long double r) {
    if (r > static_cast<long double>(UINT_MAX)) {
      return L"UINT_MAX";
    }
    if (r < 0) {
      return L"0";
    }
    wchar_t b[kBufSize];
    swprintf(b, kBufSize, L"%u", ConvertToUint(r));
    return b;
  }
  std::wstring FmtUint64(long double r) {
    if (r > static_cast<long double>(ULLONG_MAX)) {
      return L"ULLONG_MAX";
    }
    if (r < 0) {
      return L"0";
    }
    wchar_t b[kBufSize];
    swprintf(b, kBufSize, L"%llu", ConvertToUint64(r));
    return b;
  }
  std::wstring FmtFloat(long double r) {
    if (r > static_cast<long double>(FLT_MAX)) {
      return L"FLT_MAX";
    }
    if (r < -static_cast<long double>(FLT_MAX)) {
      return L"-FLT_MAX";
    }
    wchar_t b[kBufSize];
    swprintf(b, kBufSize, L"%.7g", static_cast<double>(ConvertToFloat(r)));
    return b;
  }
  std::wstring FmtDouble(long double r) {
    if (r > static_cast<long double>(DBL_MAX)) {
      return L"DBL_MAX";
    }
    if (r < -static_cast<long double>(DBL_MAX)) {
      return L"-DBL_MAX";
    }
    wchar_t b[kBufSize];
    swprintf(b, kBufSize, L"%.17g", ConvertToDouble(r));
    return b;
  }
  std::wstring FmtLongDouble(long double r) {
    // long double IS the reference type; nothing to saturate against here.
    wchar_t b[kBufSize];
    swprintf(b, kBufSize, L"%.20Lg", ConvertToLongDouble(r));
    return b;
  }
  std::wstring FmtDword(long double r) {
    if (r > static_cast<long double>(UINT_MAX)) {
      return L"UINT_MAX";
    }
    if (r < 0) {
      return L"0";
    }
    wchar_t b[kBufSize];
    swprintf(b, kBufSize, L"0x%08X", ConvertToDword(r));
    return b;
  }
  std::wstring FmtHex64(long double r) {
    if (r > static_cast<long double>(ULLONG_MAX)) {
      return L"ULLONG_MAX";
    }
    if (r < 0) {
      return L"0";
    }
    wchar_t b[kBufSize];
    swprintf(b, kBufSize, L"0x%016llX", ConvertToHex64(r));
    return b;
  }
} // namespace

bool ValidateInput() {
  if (!hInputEdit) {
    return false;
  }
  wchar_t buf[kBufSize];
  const int n = GetWindowTextW(hInputEdit, buf, static_cast<int>(kBufSize));
  if (n <= 0) {
    return false;
  }
  long double tmp;
  return ParseInput(buf, &tmp);
}

bool ConvertNumbers() {
  if (!hInputEdit) {
    return false;
  }
  wchar_t input[kBufSize];
  const int n = GetWindowTextW(hInputEdit, input, static_cast<int>(kBufSize));
  if (n <= 0) {
    SetAllOutputs(L"");
    return false;
  }
  long double ref;
  if (!ParseInput(input, &ref)) {
    LOG(WARN) << L"ConvertNumbers: cannot parse \"" << input << L"\"";
    SetAllOutputs(L"---");
    ErrorBox(mainHwnd, L"Invalid Input",
             L"\"" + std::wstring(input) +
                 L"\" is not a valid number.\n\n"
                 L"Enter a decimal (e.g. 42, -3.14, .5) "
                 L"or hex (e.g. 0xFF, -0x80) value.\n"
                 L"At most one '.' is allowed; '-' is allowed only at the start.");
    return false;
  }
  if (!std::isfinite(ref)) {
    LOG(WARN) << L"ConvertNumbers: non-finite input \"" << input << L"\"";
    SetAllOutputs(L"inf/nan");
    ErrorBox(mainHwnd, L"Invalid Input",
             L"\"" + std::wstring(input) + L"\" is not a finite number (got inf or nan).");
    return false;
  }

  SetWindowTextW(hShortOutput, FmtShort(ref).c_str());
  SetWindowTextW(hIntOutput, FmtInt(ref).c_str());
  SetWindowTextW(hLongOutput, FmtLong(ref).c_str());
  SetWindowTextW(hLongLongOutput, FmtLongLong(ref).c_str());
  SetWindowTextW(hUshortOutput, FmtUshort(ref).c_str());
  SetWindowTextW(hUintOutput, FmtUint(ref).c_str());
  SetWindowTextW(hUint64Output, FmtUint64(ref).c_str());
  SetWindowTextW(hFloatOutput, FmtFloat(ref).c_str());
  SetWindowTextW(hDoubleOutput, FmtDouble(ref).c_str());
  SetWindowTextW(hLongDoubleOutput, FmtLongDouble(ref).c_str());
  SetWindowTextW(hDwordOutput, FmtDword(ref).c_str());
  SetWindowTextW(hHexOutput, FmtHex64(ref).c_str());
  return true;
}
