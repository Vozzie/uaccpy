#ifndef __UACCPY_HEADER__
#define __UACCPY_HEADER__

#if !defined(UNICODE) || !defined(_UNICODE)
#error Please define both the UNICODE and _UNICODE preprocessor definitions in the project settings.
#endif

#ifdef UACCPY_EXPORTS
#define UACCPYEXPORTS // __declspec(dllexport)
#else
#error Please define UACCPY_EXPORTS in the project settings.
#endif

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <shlobj.h>
#include <tchar.h>

#include <initguid.h>
// IFileOperation and IShellItem interface CLSID/IID's
DEFINE_GUID(CLSID_FileOperation, 0x3ad05575, 0x8857, 0x4850, 0x92, 0x77, 0x11, 0xb8, 0x5b, 0xdb, 0x8e, 0x09);
DEFINE_GUID(IID_IFileOperation,  0x947AAB5F, 0x0A5C, 0x4C13, 0xB4, 0xD6, 0x4B, 0xF7, 0x83, 0x6F, 0xC9, 0xF8);
DEFINE_GUID(IID_IShellItem,      0x43826d1e, 0xe718, 0x42ee, 0xbc, 0x55, 0xa1, 0xe2, 0x61, 0xc3, 0x7b, 0xfe);

// FileOperation flags
#define FOF_SILENT                   0x0004
#define FOF_NOCONFIRMATION           0x0010
#define FOF_NOERRORUI                0x0400
#define FOFX_SHOWELEVATIONPROMPT     0x00040000
#define FOFX_NOCOPYHOOKS             0x00800000
#define FOFX_REQUIREELEVATION        0x10000000

// Memory functions
#define MPTR HEAP_ZERO_MEMORY
#define MemAlloc(cast,size,flags) (cast)HeapAlloc(GetProcessHeap(), (flags), (size))
#define MemFree(mem) HeapFree(GetProcessHeap(), 0, (mem))

// ExitProcess functions
#define E_UACCPY_SUCCESS 0
#define E_UACCPY_FAILED 1

// Various strings
#define S_UACCPY_ALLOW_WARNING   _T("/AllowWarning")
#define S_UACCPY_REGKEY_POLICIES _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System")
#define S_UACCPY_REGVAL_CONSENT  _T("ConsentPromptBehaviorAdmin")

// Private methods
LPWSTR AStringToWString(LPCSTR);
BOOL RegReadDWORD(HKEY, LPWSTR, LPWSTR, LPDWORD);
void _UacOperation(LPWSTR, UINT);

// Public API
#include "uaccpyapi.h"

// Satisfy compiler
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "advapi32.lib") 
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "Shlwapi.lib") 
#pragma comment(lib, "Ole32.lib")

#endif//__UACCPY_HEADER__
