#include "mode_viewAllData.h"
#include "type.h"
#include "MainFunction.h"
#include "UserInterface.h"
#include "SystemTickTimer.h"
#include "debug.h"
#include "mpu9250.h"

void mode_viewAllData(void)
{
	turnGreenLED(0);
	myPrintfUSB("############  viewAllData mode ################\n");
	setSendDataEnable(0);
	startTimerInterruptMainFunction();
	while(1){
		debugMPU9250();
		wait1msec(500);
		myPrintfUSB("=======\nlong press SW: exit\n");
		if(getSWcount()>3000) break;
	}
}
