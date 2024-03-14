#pragma once

#include <windows.h>
#include <cstdio>
#include <xinput.h>

#include "include\detours.h"
#include "include\data_types.h"
#include "Logging.h"

/////////////////////////////////////////////////////////////////////////////////////////////
//BASICS
/////////////////////////////////////////////////////////////////////////////////////////////
//Initializes mod when DLL is loaded
void Init();
BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved);

//Attaches or detaches hook function Hook_Src to SnowRunner function Src
#define DETOUR_ATTACH(Src)                                                     \
  DetourAttach(&(PVOID &)Src, (PVOID)Hook_##Src);
#define DETOUR_DETACH(Src)                                                     \
  DetourDetach(&(PVOID &)Src, (PVOID)Hook_##Src);
  
//Shamelessly borrowed
template <typename T> inline T* GetPtrToOffset(int64_t offset) {
	HMODULE base = GetModuleHandleA(NULL);

	return (T*)((BYTE*)base + offset);
}

//Defines the function Name as a pointer to the original Snowrunner function, and
//Defines the function Hook_Name as our hook for that function
#define SR_GAME_FUNCTION(Offset, R, Name, ...)						\
	inline auto *Name = GetPtrToOffset<R(__VA_ARGS__)>(Offset);		\
	R Hook_##Name(__VA_ARGS__);

#define SR_DECLARE_PTR(Offset, T, Name)                                      \
    inline auto *Name = GetPtrToOffset<T>(Offset);

//USED BY BOTH Quick Winch AND Disable Auto Reverse
SR_DECLARE_PTR(0x2EC36D8, TruckControl*, ppTruckControl) 									////UPDATED FOR 468105

/////////////////////////////////////////////////////////////////////////////////////////////
//QUICK WINCH RELATED
/////////////////////////////////////////////////////////////////////////////////////////////
inline bool WinchConnected(Vehicle * pVeh);
inline bool WeShouldHandleControls(Vehicle * pVeh, TruckControl * pTC);
#define QW_HOLD_MS			300

Vehicle * GetCurrentVehicle();

SR_DECLARE_PTR(0x2EC36C8, DriveLogic*, ppDriveLogic) 										////UPDATED FOR 468105
//RRC => Refuel, repair, cargo management
SR_DECLARE_PTR(0x2EE740D, char*, pRRC) 														////UPDATED FOR 468105
SR_DECLARE_PTR(0x2ECEAA5, char*, pRemoveCargo) 												////UPDATED FOR 468105


SR_GAME_FUNCTION(0x12D8ED0, void, GetControllerState, ControllerState *) 					////UPDATED FOR 468105
SR_GAME_FUNCTION(0xB23050, void, QuickWinch) 												////UPDATED FOR 468105

/////////////////////////////////////////////////////////////////////////////////////////////
//DAR RELATED
/////////////////////////////////////////////////////////////////////////////////////////////
//How long we have to wait, in milliseconds, between "blocked" messages
#define MIN_MESSAGE_INTERVAL 1000 

SR_GAME_FUNCTION(0xD55470, bool, ShiftGear, Vehicle*, int32_t) 								////UPDATED FOR 468105

/////////////////////////////////////////////////////////////////////////////////////////////
//FAST MODE RELATED
/////////////////////////////////////////////////////////////////////////////////////////////
SR_GAME_FUNCTION(0xB22F60, void, FastModeFlagSetter, Vehicle *, bool) 						////UPDATED FOR 468105
SR_GAME_FUNCTION(0xD55220, void, ShiftToAutoGear, Vehicle *) 								////UPDATED FOR 468105