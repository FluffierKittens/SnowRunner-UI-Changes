#include <time.h>

void GetLocalTime();

extern char timeBuf[10];

#ifdef SRUIC_NO_CONSOLE
//No logging
#define CONSOLE_LOG(...)
#else 
//Logging
#define CONSOLE_LOG(Msg, ...) 					\
	GetLocalTime();								\
	printf("[%s] " Msg "\n", timeBuf, __VA_ARGS__);
#endif