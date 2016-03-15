/**
 * @file   modeSelect.c
 * @brief  各種のモード選択を行う.<br>
 * モード一覧は以下<br>
 * - mode_calibOmega     角速度 キャリビュレーション
 * - mode_calibAccXY     加速度XYキャリビュレーション
 * - mode_calibAccZ      加速度Zキャリビュレーション
 * - mode_calibMag       地磁気 キャリビュレーション
 * - mdoe_viewAllData    9軸センサに関係するデータの表示
 * - mode_viewEEPROMData EEPROMに書き込まれているデータをすべて読み出す
 * - mode_clearOffset    角速度, 加速度, 地磁気のオフセット値を0にする.
 * @author RTCorp. Ryota Takahashi
 */

#include "modeSelect.h"

#include "LPC13xx.h"
#include "core_cm3.h"
#include "type.h"

#include "UserInterface.h"
#include "SystemTickTimer.h"
#include "debug.h"
#include "usbTransmission.h"

//動作モード毎の処理
#include "mode_calibOmega.h"
#include "mode_calibAccXY.h"
#include "mode_calibAccZ.h"
#include "mode_calibMag.h"
#include "mode_viewAllData.h"
#include "mode_viewEEPROMData.h"
#include "mode_clearOffset.h"


static void modeSelectInfo(void)
{

    myPrintfUSB("############ mode select ################\n");
    myPrintfUSB("++++++++++++++++++++++++++++++++\n");
    myPrintfUSB("  Choose the mode .\n");
    myPrintfUSB("  Short press  SW: mode change \n");
    myPrintfUSB("  Long press   SW: enter       \n");
    myPrintfUSB("++++++++++++++++++++++++++++++++\n");
    myPrintfUSB("  0 output this info.      :Green LED OFF \n");
    myPrintfUSB("  1 calib Omega       mode :Green LED ON \n");
    myPrintfUSB("  2 calib ACCXY       mode :flashing Green LED 1Hz  \n");
    myPrintfUSB("  3 calib ACCZ        mode :flashing Green LED 2Hz  \n");
    myPrintfUSB("  4 calib Mag         mode :flashing Green LED 4Hz  \n");
    myPrintfUSB("  5 viewAllData       mode :flashing Green LED 8Hz  \n");
    myPrintfUSB("  6 viewEEPROMData    mode :flashing Green LED 16Hz \n");
    myPrintfUSB("  7 clear offset      mode :flashing Green LED 32Hz \n");
}

void modeSelect(void)
{
    uint8_t md_select = 0;
    uint32_t sw_count_max = 0;
    modeSelectInfo();

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
				md_select = md_select % 8;
				myPrintfUSB("\t md_select = %d \n  ",md_select);
				switch(md_select){
					case  0:  turnGreenLED(0);
							  modeSelectInfo();
					break;
					case  1:  turnGreenLED(1);
					break;
					case  2:  flashGreenLED(500,500);
					break;
					case  3:  flashGreenLED(250,250);
					break;
					case  4:  flashGreenLED(125,125);
					break;
					case  5:  flashGreenLED(62,63);
					break;
					case  6:  flashGreenLED(31,32);
					break;
					case  7:  flashGreenLED(16,16);
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


    switch(md_select){
        case  0:
            break;
        case  1:  mode_calibOmega();
            break;
        case  2:  mode_calibAccXY();
            break;
        case  3:  mode_calibAccZ();
            break;
        case  4:  mode_calibMag();
            break;
        case  5:  mode_viewAllData();
            break;
        case  6:  mode_viewEEPROMData();
            break;
        case  7:  mode_clearOffset();
                 break;
    }

    flashGreenLED(160,40);
    wait1msec(3000);
    turnGreenLED(0);
}


