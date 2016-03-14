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
	myPrintfUSB(" make the sensor stationary state !! \n");
	myPrintfUSB(" make the sensor vertically !! \n");

	stopTimerInterruptMainFunction();
	calibAccZOffset();
}
