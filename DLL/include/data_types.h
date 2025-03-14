#pragma once

class ControllerState {
public:
	char pad_000[5632];			//0x0000				
	XINPUT_GAMEPAD X360;		//0x1600 
	char pad_160C[8];			//0x160C
	DWORD dGetStateSuccess;		//0x1614
};

class TruckAction {
public:
  char pad_0000[48];          // 0x0000
  class Vehicle *Veh;         // 0x0030
  float PowerCoef;            // 0x0038
  bool AutoGearSwitch;        // 0x003C
  char pad_003D[3];           // 0x003D
  float WheelTurn;            // 0x0040
  float Accel;                // 0x0044
  bool Handbrake;             // 0x0048 
  bool AWD;                   // 0x0049
  bool Diff;                  // 0x004A
  char pad_004B[21];          // 0x004B
  uint64_t N0000005B;         // 0x0060
  char pad_0068[8];           // 0x0068
  int32_t Gear_1;             // 0x0070
  int32_t Gear_2;             // 0x0074
  char pad_0078[56];          // 0x0078
  float N00000065;            // 0x00B0
  float N00000319;            // 0x00B4
  float N00000066;            // 0x00B8
  float N00000325;            // 0x00BC
  char pad_00C0[24];          // 0x00C0
  float N0000006A;            // 0x00D8
  float SwitchThreshold;      // 0x00DC
  int32_t NextGear;           // 0x00E0
  char pad_00E4[284];         // 0x00E4
};                          

class Vehicle {
public:
  char pad_0000[128];						// 0x0000
  class TruckAction * pTruckAction;			// 0x0080
  char pad_0088[1784];						// 0x0088
  char cEngineOnAndMore;					// 0X0780 
  char pad_081[111];						// 0x0781
  Vehicle * pTrailer;						// 0x07F0
  char pad_7F8[208];						// 0x07F8
  uint64_t pWinchConstraint;				// 0x08C8
  char pad_08D0[64];						// 0x08D0 
  bool bWinchNeedsEngine;					// 0x0910 
};

class DriveLogic
{
public:
	char pad_0000[32]; 		//0x0000
	class Vehicle * pCurVehicle; 	//0x0020
	char pad_0028[353]; 		//0x0028
	char bWinchConnect;   		//0x0189
	char bWinchView; 		//0X018A 
}; 

class TruckControl {
public:
  char pad_0000[8];                 	// 0x0000
  class Vehicle *pCurVehicle;       	// 0x0008
  char pad_0010[28];                	// 0x0010  
  bool bPauseOrMap;			// 0x002C
  char pad_002D[67];			// 0x002D
  uint64_t N0000094C; 			// 0x0070 //combine::SOUND_OBJECT
  uint64_t N0000094D; 			// 0x0078 //combine::SOUND_OBJECT
  uint64_t N0000094E; 			// 0x0080 //combine::SOUND_OBJECT
  char pad_0088[8];                 	// 0x0088
  class String *N00000950;          	// 0x0090
  char pad_0098[64];               	// 0x0098 
  bool bFunctionMenu;			// 0x00D8 			
  bool bFastMode;			// 0x00D9  
};                                                              
