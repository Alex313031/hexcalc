#ifndef HEXCALC_CONVERTERS_H_
#define HEXCALC_CONVERTERS_H_

#include "framework.h"

#include <cfloat>  // FLT_MAX, DBL_MAX
#include <climits> // INT_MAX/MIN, etc.
#include <cmath>   // std::isfinite

// Reads hInputEdit and returns true if it parses as a number (long double).
bool ValidateInput();

// Reads hInputEdit, parses it once as long double, and writes the
// formatted result of every per-type conversion into the output controls.
bool ConvertNumbers();

// Per-type converters.  The reference value is parsed once as long double
// (which is 80-bit extended precision under MinGW; same as double under
// MSVC) and each function narrows it to the target type.  Callers must
// ensure the input is finite (ConvertNumbers does this).
short ConvertToShort(const long double in);
int ConvertToInt(const long double in);
long ConvertToLong(const long double in);
long long ConvertToLongLong(const long double in);
unsigned short ConvertToUshort(const long double in);
unsigned int ConvertToUint(const long double in);
unsigned long long ConvertToUint64(const long double in);
float ConvertToFloat(const long double in);
double ConvertToDouble(const long double in);
long double ConvertToLongDouble(const long double in);
unsigned int ConvertToDword(const long double in);       // 32-bit hex view
unsigned long long ConvertToHex64(const long double in); // 64-bit hex view

#endif // HEXCALC_CONVERTERS_H_
