#include <Windows.h>
#include <TlHelp32.h>

#define SHOW_FATAL_ERROR(Msg) \
	MessageBoxW(NULL, L##Msg, L"SRUIC Injector: Fatal Error", MB_ICONERROR);

#define SRUIC_DLL_NAME L"SRUIC.dll"