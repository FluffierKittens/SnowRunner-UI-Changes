 #include "SRUIC.h"

using namespace std;

//Used to determine how long since we printed the last "blocked" message for Disable Auto Reverse
ULONGLONG lastTickCount;

//Needed for Quick Winch
bool bHoldingCancel = false;

//Needed for Fast Mode gear adjustments
uint32_t dOriginalGear = 0;
float fOriginalPowerCoef = 1.0;
bool bOriginalAuto = false;
bool bOriginalDiff = false; 

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
    if (DetourIsHelperProcess()) {
        return TRUE;
    }

	//Dll is being loaded
    if (dwReason == DLL_PROCESS_ATTACH) {
        Init();
    }
	//DLL is being unloaded
    else if (dwReason == DLL_PROCESS_DETACH) {
		//Detach our hook
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        
        DETOUR_DETACH(ShiftGear) //for DAR
		DETOUR_DETACH(GetControllerState) //for QW
		DETOUR_DETACH(QuickWinch) //for QW
		DETOUR_DETACH(FastModeFlagSetter) //For Fast Mode

        DetourTransactionCommit();

#ifndef SRUIC_NO_CONSOLE
		//And dispose of our console if we had one
        FreeConsole();
#endif
    }
    return TRUE;
}

//Initialize the mod
void Init()
{
#ifndef SRUIC_NO_CONSOLE
	//Create a console, if desired, for viewing mod information while playing
    if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
        AllocConsole();
    }
	
	FILE *file = nullptr;
    freopen_s(&file, "CONOUT$", "w", stdout);
    freopen_s(&file, "CONOUT$", "w", stderr);
#endif

    CONSOLE_LOG("SnowRunner UI Changes v0.1.13 (for version 1.594075.SNOW_DLC_14)")

	//Set our hook
    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    DETOUR_ATTACH(ShiftGear) //for DAR
	DETOUR_ATTACH(GetControllerState) //for QW
	DETOUR_ATTACH(QuickWinch) //for QW
	DETOUR_ATTACH(FastModeFlagSetter) //For Fast Mode

    DetourTransactionCommit();
	
	//Set up the counter for DAR rate limiting messages
	lastTickCount = GetTickCount64();
	
	//And announce that we're ready
    CONSOLE_LOG("Initialized")
}

//Our hook into the ShiftGear function
//FOR DAR
bool Hook_ShiftGear(Vehicle* veh, int32_t gear) 
{
    //We only care if the game is trying to shift into reverse
    if (gear == -1)
    {   //And we only care if we're in AUTO
        if (veh->pTruckAction->AutoGearSwitch == 1)
        {
            //Trying to shift from -1 to -1 (Gear_1 holds the previous gear)
            if (veh->pTruckAction->Gear_1 == -1) 
            {
				//When shifting R -> A after the truck has been moving backward, the game tries to put you in AUTO REVERSE
				//In this case we instead shift to AUTO 1/x
				//Power coefficient is 1.0 for both A and R; no need to update it
                CONSOLE_LOG("Blocking auto reverse while shifting R -> A.")
                return ShiftGear(veh, 1);
            }
			//Not sure how the TruckControl pointer could be null at this point, but safety first
			else if ((*ppTruckControl != nullptr) && (((TruckControl *)(*ppTruckControl))->bFastMode))
			{
				CONSOLE_LOG("Allowing auto reverse while in fast mode.")
				return ShiftGear(veh, gear);
			}
            else //In all other cases, the game is trying to trick you into driving off a cliff
            {
				//Don't spam with messages
				ULONGLONG curTickCount = GetTickCount64();
				if (curTickCount - lastTickCount > MIN_MESSAGE_INTERVAL)
				{
					lastTickCount = curTickCount;
					CONSOLE_LOG("Blocking auto-reverse.")
				}
                return false;
            }
        }
        else //Not in auto
        {
			CONSOLE_LOG("Using R. No problem.")
            return ShiftGear(veh, gear);
        }
    }
    else //Not trying to shift into reverse
	{
		const char * onString = "ON";
		const char * offString = "OFF";
		CONSOLE_LOG("Shifting from %d to %d. Auto: %s.", veh->pTruckAction->Gear_1, gear, veh->pTruckAction->AutoGearSwitch ? onString : offString)
		return ShiftGear(veh, gear);
	}
}

//Our hook for XBOX controller input
void Hook_GetControllerState(ControllerState * pCS)
{
	GetControllerState(pCS);
	
	//Only want to look if the call returned success	
	if (pCS->dGetStateSuccess)
	{
		TruckControl * pTC = (TruckControl *)(*ppTruckControl);
		DriveLogic * pDL = (DriveLogic *)(*ppDriveLogic);
		Vehicle * pCurVeh = GetCurrentVehicle();
			
		//Make sure we're actually in a truck
		if ((pTC != nullptr) && (pDL != nullptr) && (pCurVeh != nullptr))
		{		
			//For the quick winch cancel button
			//Only want to look if LB isn't pressed, we're in the right spot to accept input, and we don't already have the winch connected
			if (((pCS->X360.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) == 0) && WeShouldHandleControls(pCurVeh, pTC) && (WinchConnected(pCurVeh) == false))
			{
				//Only cancel if we're hitting the cancel button and we're already in winch view
				if ((pCS->X360.wButtons & XINPUT_GAMEPAD_B) && pDL->bWinchView) 
				{
					bHoldingCancel = true;
				}
				else
				{			
					if (bHoldingCancel)
					{
						//We've just released
						
						CONSOLE_LOG("Winch operation canceled")
						pDL->bWinchView = 0;
					}
					
					//No longer holding
					bHoldingCancel = false;
				}
			}
		}
	}
}

//This function works while in "remove cargo"
//Trying to cancel winch view at that time will also exit remove cargo 
void Hook_QuickWinch()
{
	DriveLogic * pDL = (DriveLogic *)(*ppDriveLogic);
	Vehicle * pCurVeh = GetCurrentVehicle();
	
	//Make sure we're in a vehicle
	if ((pDL != nullptr) && (pCurVeh != nullptr))
	{
		//Make sure we're able to use the winch at this point
		//Engine on OR autonomous winch
		if ((pCurVeh->cEngineOnAndMore & 1) || (pCurVeh->bWinchNeedsEngine == false))
		{
			//If we're not in winch view, turn it on
			if (pDL->bWinchView == 0)
			{
				pDL->bWinchView = 1;
				CONSOLE_LOG("Winch view activated.")
			}
			else //If we're already in winch view, try to connect
			{
				pDL->bWinchConnect = 1;
				CONSOLE_LOG("Winch connection attempted.")
			}
		}
	}
	//Don't call the original function
}

//Returns true if we're in a state where we might want to handle input to control a truck
//I.e. not paused, in the function menu, or in the map
//	Not in the refueling menu, repair menu, trailer store, recover prompt, removing trailers, managing cargo, or viewing a task/contract

// As of now, recover prompt, trailer store, and viewing a task can't reliably be detected
// This was causing control input to be occasionally be ignored until game was paused and resumed
// I've decided it's better to allow canceling winch view in these three situations 
// This is minor unexpected behaviour; ignored input was major unexpected behaviour
inline bool WeShouldHandleControls(Vehicle * pVeh, TruckControl * pTC)
{				
	if (pTC->bPauseOrMap)
		return false;
	if (pTC->bFunctionMenu) //Also true in crane mode and delete trailer
		return false;
	
	if (pRRC != nullptr)
	{
		//1 when in refuel, repair, or cargo management
		if (*((char *)pRRC) == 1)
			return false;
	}
	
	if (pRemoveCargo != nullptr)
	{
		//True when using "remove cargo" function
		if (*((char *)pRemoveCargo) == 1)
			return false;
	}
	
	return true;
}

inline bool WinchConnected(Vehicle * pVeh)
{
	//If we have a trailer, the trailer might be winched to something
	if (pVeh->pTrailer != nullptr)
		return (pVeh->pWinchConstraint != 0) || WinchConnected(pVeh->pTrailer);
	else
		return (pVeh->pWinchConstraint != 0);
}

Vehicle * GetCurrentVehicle() {
  if ((*ppTruckControl) == nullptr) {
    return nullptr;
  }

  return ((TruckControl *)(*ppTruckControl))->pCurVehicle;
}

void Hook_FastModeFlagSetter(Vehicle * pVehicle, bool bOn)
{
	const char * onString = "ON";
	const char * offString = "OFF";
	if (bOn)
	{
		//Switching to fast mode
		//Store the original Auto setting
		bOriginalAuto = pVehicle->pTruckAction->AutoGearSwitch;
		
		//If we're not in Auto
		if (!bOriginalAuto)
		{
			//Save the original gear and coefficient
			dOriginalGear = pVehicle->pTruckAction->Gear_2;
			fOriginalPowerCoef = pVehicle->pTruckAction->PowerCoef;
			bOriginalDiff = pVehicle->pTruckAction->Diff;
			
			//And shift to Auto
			ShiftToAutoGear(pVehicle);
			
			CONSOLE_LOG("Switching to fast mode while not in Auto. Saving gear %d and coefficient %f, and diff lock %s", dOriginalGear, fOriginalPowerCoef, bOriginalDiff ? onString : offString)
		}
	}
	else 
	{
		TruckControl * pTC = (TruckControl *)(*ppTruckControl);
		
		if (pTC != nullptr)
		{
			//We were in fast mode and now we're leaving	
			if (pTC->bFastMode)
			{
				//If we didn't enter fast mode in Auto
				if (!bOriginalAuto)
				{
					CONSOLE_LOG("Leaving fast mode and restoring gear %d and coefficient: %f, and diff lock %s", dOriginalGear, fOriginalPowerCoef, bOriginalDiff ? onString : offString)
					
					//Turn off Auto
					pVehicle->pTruckAction->AutoGearSwitch = bOriginalAuto;
					
					//And restore the original gear and coefficient 
					ShiftGear(pVehicle, dOriginalGear);
					pVehicle->pTruckAction->PowerCoef = fOriginalPowerCoef;
					pVehicle->pTruckAction->Diff = bOriginalDiff;
				}
				else //If we did enter fast mode in auto, make sure we're not leaving in AUTO REVERSE
				{
					if (pVehicle->pTruckAction->Gear_1 == -1) 
					{
						CONSOLE_LOG("Blocking auto reverse while leaving fast mode.")
						ShiftGear(pVehicle, 1);
					}
				}
			}
		}
	}
	
	//Call the base game function
	FastModeFlagSetter(pVehicle, bOn);
}