#include "mode_calibOmega.h"
#include "type.h"
#include "MainFunction.h"
#include "UserInterface.h"
#include "SystemTickTimer.h"
#include "debug.h"
#include "mpu9250.h"

void mode_calibOmega(void)
{
	turnGreenLED(0);
	myPrintfUSB("############  calib Omega mode ################\n");
	stopTimerInterruptMainFunction();
	calibOmegaOffset();
}
