#include "mode_calibAccZ.h"
#include "type.h"
#include "MainFunction.h"
#include "UserInterface.h"
#include "SystemTickTimer.h"
#include "debug.h"
#include "mpu9250.h"

void mode_calibAccZ(void)
{
	turnGreenLED(0);
	myPrintfUSB("############  calib AccZ mode ################\n");
	stopTimerInterruptMainFunction();
	calibAccZOffset();
}
