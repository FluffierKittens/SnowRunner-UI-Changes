#include "Injector.h"

using namespace std;

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    //Grab the list of running processes
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) 
	{
		SHOW_FATAL_ERROR("Failed to create process snapshot")
        return -1;
    }
    
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    //This is just needed for the first process in the snapshot
    if (!Process32First(hProcessSnap, &pe32)) 
	{ 
        CloseHandle(hProcessSnap);
		SHOW_FATAL_ERROR("Failed to handle first process in snapshot")
        return -1;
    }

    const char* exeName = "SnowRunner.exe";
    DWORD pid = 0;

	//Search all the processes until you find SnowRunner
     do {
        if (strstr(pe32.szExeFile, exeName) != NULL) {
            pid = pe32.th32ProcessID;

            break;
        }
    } while (Process32Next(hProcessSnap, &pe32)); 

    CloseHandle(hProcessSnap);

	//Make sure it was found
    if (pid == 0) 
	{
        SHOW_FATAL_ERROR("Failed to locate SnowRunner process. Is the game running?")

        return -1;
    }

	//Open the process
    HANDLE gameHandle = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION |
        PROCESS_VM_WRITE,
        false, pid); //Requesting rights to create a thread in the process, perform operation in address space of process, and write process memory. New processes created don't inherit the handle
    if (!gameHandle) 
	{
        SHOW_FATAL_ERROR("Failed to open SnowRunner process")

        return -1;
    }

    //Allocate a buffer in SnowRunner process memory for the hook DLL name
    const WCHAR* dllName = CM_DLL_NAME;
    SIZE_T memSize = (1 + lstrlenW(dllName)) * sizeof(WCHAR);
    LPWSTR bufDllName = LPWSTR(
        VirtualAllocEx(gameHandle, NULL, memSize, MEM_COMMIT, PAGE_READWRITE));
	if (!bufDllName) 
	{
		SHOW_FATAL_ERROR("Memory allocation failed")

        return -1;
	}
		
    //Store the name
    if (!WriteProcessMemory(gameHandle, bufDllName, dllName, memSize, NULL)) 
	{
		SHOW_FATAL_ERROR("Memory write failed")

        return -1;
	}

    PTHREAD_START_ROUTINE loadLibraryFunction = (PTHREAD_START_ROUTINE)(GetProcAddress(
        GetModuleHandleW(L"Kernel32"), "LoadLibraryW"));
		
	if (!loadLibraryFunction) 
	{
		//I think your PC is really broken at this point
		SHOW_FATAL_ERROR("Couldn't find LoadLibraryW")

        return -1;
	}

    //Start a thread that loads our DLL in the SnowRunner process
    HANDLE remoteThreadHandle = CreateRemoteThread(
        gameHandle, NULL, 0, loadLibraryFunction, bufDllName, 0, NULL); //Start in the game process, no security attributes, no stack size, function to start = loadLibraryFunction, dllName is the argument, no creation flags, don't save the thread ID

    if (!remoteThreadHandle) {
        SHOW_FATAL_ERROR("Failed to create remote thread")

        return -1;
    }
	
	//Wait to see how the library load goes
	WaitForSingleObject(remoteThreadHandle, INFINITE);
	
	//Once it's done, check its return value
    DWORD exitCode = 0;
    GetExitCodeThread(remoteThreadHandle, &exitCode);
	
	if (exitCode == 0)
	{
		SHOW_FATAL_ERROR("Failed to inject DLL. Is " CM_DLL_NAME " in the same folder as SnowRunner.exe?")

        return -1;
	}
	
//If there's no debug console, we at least announce success
#ifdef DAR_NO_CONSOLE
	MessageBoxW(NULL, CM_DLL_NAME " successfully injected", L"DAR Injector", MB_ICONINFORMATION);
#endif

	//If we made it this far, DllMain is now executing in the SnowRunner process
	return 0;
}

