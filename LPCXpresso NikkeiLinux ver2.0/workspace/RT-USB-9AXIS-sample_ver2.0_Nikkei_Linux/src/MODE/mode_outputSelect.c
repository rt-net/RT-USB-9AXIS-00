#include "mode_outputSelect.h"
#include "type.h"
#include "MainFunction.h"
#include "UserInterface.h"
#include "SystemTickTimer.h"
#include "debug.h"
#include "mpu9250.h"


static void outputModeSelectInfo(void)
{

    myPrintfUSB("############ output mode select ################\n");
    myPrintfUSB("++++++++++++++++++++++++++++++++\n");
    myPrintfUSB("  Choose the mode .\n");
    myPrintfUSB("  Short press  SW: mode change \n");
    myPrintfUSB("  Long press   SW: enter       \n");
    myPrintfUSB("++++++++++++++++++++++++++++++++\n");
    myPrintfUSB("  0 output this info.         :Green LED OFF \n");
    myPrintfUSB("  1 UART 57600 USB   BINARY 9AXIS mode :Green LED ON \n");
    myPrintfUSB("  2            USB   ASCII  9AXIS mode :flashing Green LED 1Hz  \n");
}


void mode_outputSelect(void)
{
	uint8_t md_select = 0;
	uint32_t sw_count_max = 0;
	turnGreenLED(0);
	setSendDataEnable(0);

	outputModeSelectInfo();

	while(1)
	{
		wait1msec(1);
		if( getSWcount() > 0 ) //SWでモード選択
		{
			while(getSWcount() != 0 )
			{
				sw_count_max = getSWcount();
				wait1msec(1);
				if(sw_count_max >3000) break;
			};
			//スイッチ短押し
			if(sw_count_max <3000)
			{
				md_select++;
				md_select = md_select % 3;
				myPrintfUSB("\t md_select = %d \n  ",md_select);
				switch(md_select){
					case  0:  turnGreenLED(0);
							  outputModeSelectInfo();
					break;
					case  1:  turnGreenLED(1);
					break;
					case  2:  flashGreenLED(800,200);
					break;
				}
			}
			else //スイッチ長押し
			{
				break;
			}
		}
	}//end while

	myPrintfUSB("\t　enter\n");
	flashGreenLED(20,80);
	wait1msec(3000);
	turnGreenLED(0);

	setOutputMode(md_select-1);

	flashGreenLED(160,40);
	wait1msec(3000);
	turnGreenLED(0);

}
