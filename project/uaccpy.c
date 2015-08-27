
#include "uaccpy.h"

void _UacOperation(LPWSTR lpszCmdLine, UINT uOperation)
{
	BOOL bInit = FALSE;
	WCHAR * pSource = NULL;
	WCHAR * pFolder = NULL;
	WCHAR * pTarget = NULL;
	WCHAR * pTmp;
	UINT uExitCode = E_UACCPY_FAILED;
	// Parse arguments...
	if(!lpszCmdLine || *lpszCmdLine != '"')
		goto hell;
	// start first quoted string
	pTmp = pSource = lpszCmdLine + 1;
	if(NULL == (pTmp = wcschr(pTmp, '"')))
		goto hell;
	// end first quoted string with zero
	*pTmp++ = 0;
	if(uOperation != UACOP_DELETE)
	{
		// search start of second string
		if(NULL == (pTmp = wcschr(pTmp, '"')))
			goto hell;
		if(!*++pTmp)
			goto hell;
		// start second quoted string
		pFolder = pTmp;
		if(NULL == (pTmp = wcsrchr(pTmp, '\\')))
			goto hell;
		if(pFolder == pTmp)
			goto hell;
		// end second string with zero
		*pTmp++ = 0;
		// Start of third string
		pTarget = pTmp;
		if(NULL == (pTmp = wcsrchr(pTmp, '"')))
			goto hell;
		// end last string with zero
		*pTmp++ = 0;
	}
	// has no allow warning flag and needs consent?
	if(NULL == _wcsistr(pTmp, S_UACCPY_ALLOW_WARNING) 
	&& NeedsUserConsent())
		goto hell;
	// Init ole
	if(!(bInit = SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))))
		goto hell;
	// Copy operation
	if(!UacOperation(pSource, pFolder, pTarget, uOperation))
		goto hell;
	uExitCode = E_UACCPY_SUCCESS;
hell:
	if(bInit) CoUninitialize();
	ExitProcess(uExitCode);
}

LPWSTR AStringToWString(LPCSTR lpszSource)
{
	WCHAR * lpszTarget;
	int ccLength, ccConverted; 

	// check arguments
	if(lpszSource == NULL 
	|| 0 == ( ccLength = lstrlenA(lpszSource) ) 
	){
		return NULL;
	}

	// get required buffer size to convert to ansi
	if( 0 == ( ccConverted = MultiByteToWideChar(CP_ACP, 0, lpszSource, ++ccLength, NULL, 0) ) )
	{
		return NULL;
	}

	// allocate memory for string
	if( NULL == ( lpszTarget = MemAlloc(LPWSTR, (ccConverted * sizeof(WCHAR)), MPTR ) ) )
	{
		return NULL;
	}

	// convert to ansi
	if( ccConverted != MultiByteToWideChar(CP_ACP, 0, lpszSource, ccLength, lpszTarget, ccConverted) )
	{
		MemFree(lpszTarget);
		return NULL;
	}

	return lpszTarget;
}

void UACCPYEXPORTS CALLBACK CopyW(HWND hWnd, HINSTANCE hInstance, LPWSTR lpszCmdLine, int nCmdShow)
{
	_UacOperation(lpszCmdLine, UACOP_COPY);
}

void UACCPYEXPORTS CALLBACK DeleteW(HWND hWnd, HINSTANCE hInstance, LPWSTR lpszCmdLine, int nCmdShow)
{
	_UacOperation(lpszCmdLine, UACOP_DELETE);
}

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD dwReason, LPVOID lpvReserved)
{
	return TRUE;
}

void UACCPYEXPORTS CALLBACK MoveW(HWND hWnd, HINSTANCE hInstance, LPWSTR lpszCmdLine, int nCmdShow)
{
	_UacOperation(lpszCmdLine, UACOP_MOVE);
}

BOOL UACCPYAPI NeedsUserConsent(void)
{
	DWORD dwValue;
	if(!RegReadDWORD(HKEY_LOCAL_MACHINE, S_UACCPY_REGKEY_POLICIES, S_UACCPY_REGVAL_CONSENT, &dwValue))
		return FALSE;
	if(dwValue & 2)
		return TRUE;
	return FALSE;
}

BOOL RegReadDWORD(HKEY hRoot, LPWSTR lpszKey, LPWSTR lpszValueName, LPDWORD lpdwValue)
{
	HKEY hKey;
	DWORD dwType;
	DWORD cbResult;
	BOOL bSuccess = FALSE;
	if(SUCCEEDED(RegOpenKeyExW(hRoot, lpszKey, 0, KEY_QUERY_VALUE | KEY_READ, &hKey)))
	{
		dwType = REG_DWORD;
		cbResult = sizeof(DWORD);
		if(SUCCEEDED(RegQueryValueExW(hKey, lpszValueName, NULL, &dwType, (LPBYTE)lpdwValue, &cbResult)))
			bSuccess = TRUE;
		RegCloseKey(hKey);
	}
	return bSuccess ;
}

BOOL UACCPYAPI UacOperationA(LPSTR lpszSource, LPSTR lpszFolder, LPSTR lpszTarget, UINT uOperation)
{
	LPWSTR lpwszSource = NULL, lpwszFolder = NULL, lpwszTarget = NULL;
	BOOL bResult = FALSE;
	switch(uOperation)
	{
		case UACOP_COPY:
		case UACOP_MOVE:
			lpwszFolder = AStringToWString(lpszFolder);
			if(!lpwszFolder) goto hell;
			lpwszTarget = AStringToWString(lpszTarget);
			if(!lpwszTarget) goto hell;
		case UACOP_DELETE:
			lpwszSource = AStringToWString(lpszSource);
			if(!lpwszSource) goto hell;
			break;
		default: 
			return FALSE;
	}
	bResult = UacOperationW(lpwszSource, lpwszFolder, lpwszTarget, uOperation);
hell:
	if(lpwszSource != NULL) MemFree(lpwszSource);
	if(lpwszFolder != NULL) MemFree(lpwszFolder);
	if(lpwszTarget != NULL) MemFree(lpwszTarget);
	return bResult;
}

BOOL UACCPYAPI UacOperationW(LPWSTR lpszSource, LPWSTR lpszFolder, LPWSTR lpszTarget, UINT uOperation)
{
	IFileOperation *pFileOper = NULL;
	IShellItem     *psiSource = NULL;
	IShellItem     *psiTarget = NULL;
	BOOL			bSuccess = FALSE;
	if(FAILED(CoCreateInstance(&CLSID_FileOperation, NULL, CLSCTX_ALL, &IID_IFileOperation, (void**)&pFileOper)))
		goto hell;
	if(FAILED(pFileOper->lpVtbl->SetOperationFlags(pFileOper, FOF_NOCONFIRMATION | FOF_SILENT | FOFX_SHOWELEVATIONPROMPT | FOFX_NOCOPYHOOKS | FOFX_REQUIREELEVATION | FOF_NOERRORUI )))
		goto hell;
	if(FAILED(SHCreateItemFromParsingName(lpszSource, NULL, &IID_IShellItem, (void**)&psiSource)))
		goto hell;
	switch(uOperation)
	{
		case UACOP_COPY:
			if(FAILED(SHCreateItemFromParsingName(lpszFolder, NULL, &IID_IShellItem, (void**)&psiTarget)))
				goto hell;
			if(FAILED(pFileOper->lpVtbl->CopyItem(pFileOper, psiSource, psiTarget, lpszTarget, NULL)))
				goto hell;
			break;
		case UACOP_DELETE:
			if(FAILED(pFileOper->lpVtbl->DeleteItem(pFileOper, psiSource, NULL)))
				goto hell;
			break;
		case UACOP_MOVE:
			if(FAILED(SHCreateItemFromParsingName(lpszFolder, NULL, &IID_IShellItem, (void**)&psiTarget)))
				goto hell;
			if(FAILED(pFileOper->lpVtbl->MoveItem(pFileOper, psiSource, psiTarget, lpszTarget, NULL)))
				goto hell;
			break;
		default:
			goto hell;
	}
	if(SUCCEEDED(pFileOper->lpVtbl->PerformOperations(pFileOper)))
		bSuccess = TRUE;
hell:
	if(psiSource) psiSource->lpVtbl->Release(psiSource);
	if(psiTarget) psiTarget->lpVtbl->Release(psiTarget);
	if(pFileOper) pFileOper->lpVtbl->Release(pFileOper);
	return bSuccess;


}
