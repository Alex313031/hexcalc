// {{NO_DEPENDENCIES}}
// For #define-ing static resources for resource script file(s).
// Used by resource.rc

// clang-format off

/* Icons */
#define IDI_MAIN                    101 /* 32x32 & 48x48 icon */
#define IDI_SMALL                   102 /* Small 16x16 icon */
#define IDI_ABOUT                   103 /* About Dialog icon */

/* Main application resource, also used to attach menu */
#define IDR_MAIN                    110

/* Dialogs */
#define IDD_ABOUTDLG                120

/* Menu items */
#define IDM_ABOUT                   200
#define IDM_EXIT                    201
#define IDM_HELP                    202
#define IDM_RESERVED                203

/* Buttons */
#define IDC_CONVERT                 220
#define IDC_ABOUT                   221

/* Edit Controls */
#define IDC_INPUT                   222

/* Static Controls (signed -> unsigned -> float -> hex) */
#define IDC_SHORT                   223
#define IDC_INT                     224
#define IDC_LONG                    225
#define IDC_LONGLONG                226
#define IDC_UINT                    227
#define IDC_UINT64                  228
#define IDC_FLOAT                   229
#define IDC_DOUBLE                  230
#define IDC_LONGDOUBLE              231
#define IDC_DWORD                   232
#define IDC_HEX                     233

// Not in a UI menu, just keyboard accelerator
#define IDM_UNDO                    250

// Dev menu item, test debug trap
#define IDM_TESTTRAP                280

// For resources to be loaded without an ID from the system.
#ifndef IDC_STATIC
 #define IDC_STATIC                 -1
#endif // IDC_STATIC
// clang-format on
