#include "mode_viewEEPROMData.h"
#include "type.h"
#include "MainFunction.h"
#include "UserInterface.h"
#include "SystemTickTimer.h"
#include "debug.h"
#include "EEPROM_24LC16BT_I.h"

void mode_viewEEPROMData(void)
{
	turnGreenLED(0);
	myPrintfUSB("############  viewEEPROMData mode ################\n");
	setSendDataEnable(0);
	stopTimerInterruptMainFunction();
	printAllData();
}
