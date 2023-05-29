#include "logging.h"

char timeBuf[10];	

void GetLocalTime()
{
	time_t rawTime;
	struct tm lTime;	
	
	//Get the current time
	time(&rawTime);
	//Account for time zones
	localtime_s(&lTime, &rawTime);
	//Format it nicely
	strftime (timeBuf, sizeof(timeBuf), "%T", &lTime);
}