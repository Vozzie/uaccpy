#ifndef __UACCPYAPI_HEADER__
#define __UACCPYAPI_HEADER__

#include <windows.h>

/**
 * One should call CoInitializeEx once in order for the functions to work.
 * eg.
 *   CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)
 *
 */

#define UACCPYAPI __stdcall
#ifndef UACCPY_EXPORTS
#define UACCPYEXPORTS __declspec(dllimport)
#endif

#define UACOP_COPY 0
#define UACOP_DELETE 1
#define UACOP_MOVE 2

#ifdef __cplusplus
extern "C" {
#endif

	BOOL UACCPYEXPORTS UACCPYAPI NeedsUserConsent(void);
	BOOL UACCPYEXPORTS UACCPYAPI UacOperationW(LPWSTR, LPWSTR, LPWSTR, UINT);

#ifdef __cplusplus
} // extern "C"
#endif

#if defined(UNICODE) || defined(_UNICODE)
#define UacOperation UacOperationW 
#else
#define UacOperation UacOperationA
#endif

#define UacCopy(source,folder,name) UacOperation((source),(folder),(name),UACOP_COPY)
#define UacDelete(source) UacOperation((source),NULL,NULL,UACOP_DELETE)
#define UacMove(source,folder,name) UacOperation((source),(folder),(name),UACOP_MOVE)

#endif//__UACCPYAPI_HEADER__
