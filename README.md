# UI Changes for SnowRunner
A collection of UI changes for the game SnowRunner.

## Features

### AUTO REVERSE
Disables AUTO REVERSE. The only way to reverse is now with the dedicated R gear. NOTE: This does not apply while in fast mode (aka "control evacuator").

### Quick Winch

1. Press the quick winch button (Y) once to activate winch view. 
![Winch view](https://github.com/FluffierKittens/SnowRunner-UI-Changes/assets/130815166/2692b2bb-fc6a-41c1-9cf6-b159dacdf568)
2. With winch view on, use the camera controls to confirm that you've selected the desired winch point. 
3. If winch view is on and you no longer wish to connect your winch, press the cancel button (B).
4. With winch view on, press the quick winch button again to connect your winch.

### Fast Mode (aka "control evacuator")

Entering fast mode shifts your truck into AUTO gear. AUTO REVERSE is enabled, allowing full forward and reverse movement of the truck without exiting fast mode. 

The game's default behaviour is that entering fast mode in L, H, or R leaves you unable to change direction, due to the gear shift UI being replaced. 

Upon exiting fast mode, your original gear setting is restored.

## Known Issues

The quick winch cancel button (B) is only compatible with Xbox controllers at this time. Testing indicates that [DS4Windows](https://ds4-windows.com/) with [HidHide](https://ds4-windows.com/download/hidhide/), using Xbox 360 emulation mode, also works.


This mod is only compatible with SnowRunner.exe version 1.291006.SNOW_DLC_10

## Installation
1. Download the latest release from [here](https://github.com/FluffierKittens/SnowRunner-UI-Changes/releases).
2. Extract *SRUIC.dll* and *SRUICInjector.exe* files from the archive. 
3. Place *SRUIC.dll* in the game folder where SnowRunner.exe is located. 
4. Place *SRUICInjector.exe* anywhere that you find convenient. The game folder is just fine.

## Usage
1. Start SnowRunner
2. Run *SRUICInjector.exe* and make sure you get a message indicating success. 
3. Start a new game or open an existing save, and have fun!

## Building
 
**With only the code provided, this project will not build.**
 
This project requires the Microsoft Detours library. You will need to place *detours.lib* (x64 version) in the *DLL\lib* folder and *detours.h* in the *DLL\include* folder. 
 
For help obtaining these libraries, see the [Detours FAQ](https://github.com/microsoft/Detours/wiki/FAQ).

In the future I may look into updating the build process so that Detours is automatically installed if needed, but for the moment I'm focusing on adding additional features.
