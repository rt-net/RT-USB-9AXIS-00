#include "mode_calibAccXY.h"
#include "type.h"
#include "MainFunction.h"
#include "UserInterface.h"
#include "SystemTickTimer.h"
#include "debug.h"
#include "mpu9250.h"

void mode_calibAccXY(void)
{
	turnGreenLED(0);
	myPrintfUSB("############  calib AccXY mode ################\n");
	myPrintfUSB(" make the sensor stationary state !! \n");
	myPrintfUSB(" make the sensor horizontally !! \n");

	stopTimerInterruptMainFunction();
	calibAccXYOffset();
}
