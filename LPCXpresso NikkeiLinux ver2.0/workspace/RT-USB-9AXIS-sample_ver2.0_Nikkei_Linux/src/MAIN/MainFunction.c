/**
 * @file   MainFunction.c
 * @brief  データの送信を行う<br>
 *         10msec毎に割り込み関数で処理を実行
 *
 * @author RTCorp. Ryota Takahashi
 */

#include "LPC13xx.h"
#include "core_cm3.h"
#include "type.h"

#include "timer.h"
#include "pwm.h"
#include "MainFunction.h"
#include "UserInterface.h"
#include "uart.h"
#include "SystemTickTimer.h"

#include "mpu9250.h"
#include "debug.h"
#include "uart.h"
#include "tinyMathFunction.h"
#include "usbTransmission.h"
#include "EEPROM_24LC16BT_I.h"

volatile static uint8_t flag_send_data_enable = 0;  //タイマ割り込み関数の中でデータを送信するかのフラグ
volatile static uint8_t output_mode = 1;            //データを出力する形式
volatile static uint32_t time;                      //起動してからの時間[msec]を格納するための変数

/**
 * MainFunction.c内の関数を使用できるようにする<br>
 * MainFunction.c内に記述されている割り込み関数
 * TIMER32_1_IRQHandler()を有効にする.
 *
 * @param void
 * @return void
 */
void startTimerInterruptMainFunction(void)
{
	startTimer32_1();
}

/**
 * MainFunction.c内の関数を使用できるようにする<br>
 * MainFunction.c内に記述されている割り込み関数
 * TIMER32_1_IRQHandler()を停止させる.
 *
 * @param void
 * @return void
 */
void stopTimerInterruptMainFunction(void)
{
	stopTimer32_1();
}

/**
 * 機体データの送信をするかどうかを選択
 *
 * @param en 1:送信 0:送信じ禁止
 * @return void
 */
void setSendDataEnable(uint8_t en)
{
	flag_send_data_enable = en;
}

/**
 * EEPROMに出力モードを書き込む<br>
 *
 * @param mode 書き込む動作モード
 */
void writeEEPROMOutputMode(uint8_t mode){
	write1byteEEPROM(1,0,mode);
}

/**
 * EEPROMから出力モードを読み出す<br>
 *
 * @return EEPROMから読み出したモードの値
 */
uint8_t readEEPROMOutputMode(void){
	return read1byteEEPROM(1,0);
}

/**
 * アウトプットモードをセットする.<br>
 * - OUTPUT_MODE_57600_BINARY_9AXIS USBとUARTから9軸センサのデータをbinary形式で出力
 * - OUTPUT_MODE_230400_ASCII_9AXIS USBとUARTから9軸センサのデータをascii文字列,カンマ区切りで出力
 *
 */
void setOutputMode(uint8_t mode){
	output_mode = mode;

	switch(mode){
		case OUTPUT_MODE_57600_BINARY_9AXIS:
			UARTInit(57600);
			writeEEPROMOutputMode(mode);
			break;
		case OUTPUT_MODE_230400_ASCII_9AXIS:
			UARTInit(230400);
			writeEEPROMOutputMode(mode);
			break;
		default:
			output_mode = 0;
			UARTInit(57600);
			writeEEPROMOutputMode(OUTPUT_MODE_57600_BINARY_9AXIS);
			break;
	}
};

/**
 * 10msec毎に行われる処理 <br>
 *          <li>各種情報をUART,USBで送信 </li>
 *         </ul>
 * @param  void
 * @return void
 */
void TIMER32_1_IRQHandler(void)
{
	static uint8_t timestamp;
	static float deg = 0.0;
	float omega[3];

	LPC_TMR32B1->IR=0x08; //clear interrupt flag


	if(timestamp == 0xff) timestamp = 0;
	else timestamp++;

	//9軸センサのデータの更新
	updateMPU9250Data();
	updateAK8963Data();

	//9軸センサの観測値を送信用配列に格納

	getOmega(&omega[0]);

	if(isStop() == 0)
	{
		deg += 0.01* omega[2];
		while(deg < 0){
			deg += 2*PI;
		}
		while(deg > 2*PI)
		{
			deg -= 2*PI;
		}
		while(deg > PI)
		{
			deg -= 2*PI;
		}
	}

	if(timestamp % 10 == 0 && isUSBConfig() == 1 && flag_send_data_enable == 1 ) //USB接続確立時
	{
		myPrintfUSB("%f\n",RAD2DEG(deg));
	}
}

/******************************************************************************
**                            End Of File
******************************************************************************/
