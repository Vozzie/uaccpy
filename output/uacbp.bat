@Echo Off

REM ***************************************************************************

If Exist "%PROGRAMFILES(X86)%" (
	GoTo x64
) Else (
	GoTo x32
)

:x64
Set DLL=uaccpy64.dll
GoTo Execute

:x86
Set DLL=uaccpy32.dll
GoTo Execute

REM ***************************************************************************
REM ***************************************************************************

:Execute
If Not Exist "%DLL%" (
	Echo Could not find %DLL%
	GoTo Usage
)
If [%1]==[] GoTo MissingArgument
If [%2]==[] GoTo MissingArgument
If Not Exist %2 (
	Echo Could not find %2
	GoTo Usage
)
If /I Not "%1"=="DELETE" (
	If /I Not "%1"=="MOVE" (
		If /I Not "%1"=="COPY" (
			Echo Argument %1 invalid.
			GoTo Usage
		)
	)
	If [%3]==[] GoTo MissingArgument
)

rundll32.exe %DLL%,%1 %2 %3

GoTo End

REM ***************************************************************************
REM ***************************************************************************

:MissingArgument
Echo Argument missing.
GoTo Usage

:Usage
Echo.
Echo Usage:
Echo.
Echo    uacbp.bat Copy source target [/AllowWarning]
Echo    uacbp.bat Move source target [/AllowWarning]
Echo    uacbp.bat Delete target [/AllowWarning]
Echo.

:End

REM ***************************************************************************
