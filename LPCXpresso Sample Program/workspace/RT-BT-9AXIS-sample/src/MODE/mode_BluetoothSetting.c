/**
 * @file   mode_BluetoothSetting.c
 * @brief  Bluetoothモジュールのボーレート等を設定する.<br>
 *   //設定項目//<br>
 *   ボーレート:115200bps<br>
 *   動作モード:スレーブモード<br>
 *   設定変更後リブートコマンドを送る.<br>
 *
 * @author RTCorp. Ryota Takahashi
 */

#include "mode_BluetoothSetting.h"
#include "type.h"
#include "MainFunction.h"
#include "UserInterface.h"
#include "SystemTickTimer.h"
#include "debug.h"


void mode_BluetoothSetting(void)
{
	turnLED(0);
	myPrintfUART("############ Bluetooth setting mode ################\n");
	wait1msec(300);
	myPrintfUART("$$$");    //コマンドモードに入る
	wait1msec(2000);
	myPrintfUART("SM,0\r");   //動作モードをスレーブモードに変更
	wait1msec(2000);
	myPrintfUART("SU,115K\r");//ボーレートを115200bpsに変更
	wait1msec(2000);
	//myPrintfUART("SN,RT-BLUETOOTH-9AXIS\r");//デバイス名の変更
	wait1msec(2000);
	myPrintfUART("R,1\r"); //リブート
	wait1msec(2000);

	while(1)
	{
		turnLED(1);
		//モード選択へ遷移
		if( getSWcount() > 1000)
		{
			myPrintfUART("\t return mode select \n");
			if(getSWcount() == 0) break;
		}
	}
}

/******************************************************************************
**                            End Of File
******************************************************************************/
