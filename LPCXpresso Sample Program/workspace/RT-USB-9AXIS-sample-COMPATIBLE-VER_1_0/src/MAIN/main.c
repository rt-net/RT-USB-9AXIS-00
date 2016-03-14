/**
 * @file   main.c
 * @brief  USB出力9軸IMUセンサモジュール(ver2.0)のサンプルプログラム<br>
 *         本プログラムではver1.0のものと全く互換の出力が出るように記述されている.　<br>
 *         mpu9250.cで#define VER_1_0_COMPATIBLENO_MODE_SELECTを記述し, <br>
 *         MainFunction.c内で#define VER_1_0_COMPATIBLE_MPU9150_MAGを記述すると　<br>
 *         地磁気センサの出力がver1.0のセンサボードと同じ仕様になる.　<br>
 *         注)mpu9150とmpu9250では地磁気センサの出力分解能が異なる <br>
 *            本プログラムはmpu9250用だがVER_1_0_COMPATIBLE_MPU9150_MAG<br>
 *            をdefineするかどうかで出力の形式を選択可能
 * @author RTCorp. Ryota Takahashi
 */

#define VER_1_0_COMPATIBLE_NO_MODE_SELECT

//OHTER
#include "LPC13xx.h"
#include "core_cm3.h"
#include "type.h"
//PERIHERAL
#include "ad.h"
#include "debug.h"
#include "i2c.h"
#include "io.h"
#include "pwm.h"
#include "SystemTickTimer.h"
#include "timer.h"
#include "usbTransmission.h"
#include "uart.h"
//MAIN
#include "MainFunction.h"
#include "mpu9250.h"
#include "UserInterface.h"
#include "EEPROM_24LC16BT_I.h"

//MODE
#include "modeSelect.h"
//USB
#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "cdc.h"
#include "cdcuser.h"

/////プロトタイプ宣言//////////////
void init(void);
/////////////////////////////

/**
 * main関数(エントリポイント)<br>
 * main内では最初に各ペリフェラルの初期化を行い
 * 次に, USBへの接続を試み, データの送信を開始する.
 * 基板上のタクトスイッチが3秒以上長押しされると
 * modeSelect()関数が呼ばれキャリビュレーションや, データを
 * 表示確認するモードへ移行する.
 *
 */
int main (void)
{
	init();

	tryUSBConnect();        //USB接続を試みる
	setSendDataEnable(1); //データを送信開始

	while(1)
    {
    	if(isUSBConfig() == 1) flashRedLED(300,300);//USBの接続使用
    	else flashRedLED(100,100); //USBの接続不使用


        #ifndef VER_1_0_COMPATIBLE_NO_MODE_SELECT
    	if(getSWcount()>3000){
			while(getSWcount() != 0);
    		setSendDataEnable(0);
			stopTimerInterruptMainFunction();
			//LEDを一回だけ点滅させる
			wait1msec(500);
			turnRedLED(1);
			wait1msec(500);
			turnRedLED(0);
			wait1msec(500);
			flashRedLED(30,30);
			//モードセレクトを呼び出す
			//ver1.0と互換性を持たせるのでモード選択は行わない


			modeSelect();
			startTimerInterruptMainFunction();
			setSendDataEnable(1); //データを送信開始
		}
		#endif
    }

	return 0;
}

/**
 * 各種ペリフェラルの初期化
 *
 * @param  void
 * @return void
 */
void init(void)
{
	//IOポートの初期化
	initIO();
	//LEDを一回だけ点滅させる
	turnRedLED(1);
	wait1msec(500);
	turnRedLED(0);
	wait1msec(500);
	//各ペリフェラルの初期化
	USBIOClkConfig();
	initTimer32_0();
	initTimer32_1();
	USB_Init();
	UARTInit(57600);
	while (!I2CInit((uint32_t)I2CMASTER)); // I2Cの初期化が終わるまで待つ
	//9軸センサの初期化
	initMPU9250();
	initAK8963();
	initOffsetByEEPROM();
	//UIの初期化
	initUI();
	startTimerInterruptMainFunction();

}

/******************************************************************************
**                            End Of File
******************************************************************************/
