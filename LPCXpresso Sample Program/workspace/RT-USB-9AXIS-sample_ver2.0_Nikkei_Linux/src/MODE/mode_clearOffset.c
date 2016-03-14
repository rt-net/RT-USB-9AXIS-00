
#include "mode_clearOffset.h"
#include "type.h"
#include "MainFunction.h"
#include "UserInterface.h"
#include "SystemTickTimer.h"
#include "debug.h"
#include "EEPROM_24LC16BT_I.h"

void mode_clearOffset(void)
{
	int16_t vec[3] = {0,0,0};
	turnGreenLED(0);
	myPrintfUSB("############  clearOffset mode ################\n");
	setSendDataEnable(0);
	stopTimerInterruptMainFunction();

	writeEEPROMOffsetAccInt(&vec[0]);
	writeEEPROMOffsetOmegaInt(&vec[0]);
	writeEEPROMOffsetMagInt(&vec[0]);
}



