/**
 * @file   UserInterface.c
 * @brief  LED,SW,電源電圧の監視等の機能の実装 <br>
 *
 * @author RTCorp. Ryota Takahashi
 */

#include "LPC13xx.h"
#include "core_cm3.h"
#include "timer.h"
#include "type.h"
#include "pwm.h"
#include "debug.h"
#include "UserInterface.h"
#include "ad.h"

//LEDが点滅状態か常時点灯状態かのフラグ 1:点滅 　0:常時点灯(消灯も含む)
volatile static uint8_t flag_flash_LED = 0;
//SWの押下状態のカウント
volatile static uint16_t SW_count = 0;
//LEDの点滅用カウント
volatile static uint16_t LED_count   = 0;
//経過時間のカウント
volatile static uint32_t elapsed_time_count = 0;
//LEDの点滅時間のon/off時間のカウント
volatile static uint16_t on_count_LED    = 0;
volatile static uint16_t off_count_LED   = 1;
//電池の電圧値[V]
volatile static float voltage = 0.0;


/**
 * LEDの点滅, AD変換でのバッテリーの電圧取得, スイッチの押下状態の監視<br>
 * 機能の初期化 <br>
 * この関数は各ペリフェラルの初期化後に呼ぶこと <br>
 *
 * この関数を呼ぶと <br>
 *         <ul>
 *          <li>getSWcount()</li>
 *          <li>getSWcount() </li>
 *          <li>getElapsedTime() </li>
 *          <li>flashLED() </li>
 *          <li>getVoltage()</li>
 *         </ul>
 * が有効となる.
 *
 * @param  void
 * @return void
 */
void initUI(void)
{
	startTimer32_0();
}

/**
 * 右スイッチの押下状態取得
 *
 * @param void
 * @return 1:押している 0:押していない
 */
uint8_t getStateSW(void)
{
	uint32_t state;
	state = (LPC_GPIO0->DATA & 0x2)>>1;
	state = 1 - state;
	return state;
}

/**
 * 右スイッチの押下時間を返す
 *
 * @param void
 * @return 右スイッチを押してた 秒数 [msec]
 */
uint16_t getSWcount(void)
{
	return SW_count;
}

/**
 * initUI()を呼んでからの経過時間を返す.
 * elapsed_time_count * 1msec
 *
 * @param void
 * @return 経過した秒数 [msec]
 */
uint32_t getElapsedTime(void)
{
	return elapsed_time_count;
}

/**
 * 経過した秒数をリセットする
 *
 * @param  void
 * @return void
 */
void resetElapsedTime(void)
{
	elapsed_time_count = 0;
}

/**
 *  LEDの点灯と消灯
 *
 * @param  state 1:点灯 0:消灯
 * @return void
 */
static void setStateLED(uint8_t state)
{
	if(state == 1)
	{
		LPC_GPIO2->DATA |= 0x0001; //LED on
	}
	else
	{
		LPC_GPIO2->DATA &= ~0x0001; //LED off
	}
}

/**
 * LEDの点灯と消灯.  LEDが点滅状態で
 * 呼んだ場合は常時点灯(消灯)状態になる.
 *
 * @param  state 1:点灯 0:消灯
 * @return void
 */
void turnLED(uint8_t state)
{
	flag_flash_LED = 0;
	setStateLED(state);
}

/**
 * LEDを指定した時間間隔で点滅させる <br>
 * 例.LEDを100msecの間on, 50msecの間offで点滅させるには <br>
 *    on_count = 100, off_count = 50 <br>
 * LEDを常時点灯する場合は <br>
 *    turnLED(1) <br>
 * LEDを消灯する場合は <br>
 *    turnLED(0) <br>
 *
 * @param  on_count  [msec]
 * @param  off_count [msec]
 * @return void
 */
void flashLED(uint16_t on_count, uint16_t off_count)
{
	flag_flash_LED = 1;
	on_count_LED    = on_count;
	off_count_LED   = off_count;
}

/**
 * 電源の電圧取得
 *
 * @param void
 * @return モーターの電圧
 */
float getVoltage(void)
{
	return voltage;
}

/**
 * UserInterface内の関数を使えるようにするための割り込み関数<br>
 * 1msec毎に実行される
 *
 * @param void
 * @return void
 */
void TIMER32_0_IRQHandler(void)
{
	int16_t ad_val[8];
	LPC_TMR32B0->IR=0x08; //clear interrupt flag

	if(flag_flash_LED == 1)
	{
		//LEDの点滅
		if(LED_count < on_count_LED ) setStateLED(1);
		else setStateLED(0);
		if( (on_count_LED + off_count_LED) < LED_count ) LED_count = 0;
		LED_count ++;
	}

	//時間経過
	elapsed_time_count ++;

	//右スイッチの押下
	if(getStateSW() == 1) SW_count ++;
	else SW_count = 0;

	//AD変換と電池の電圧計算
	storeAD2Array(&ad_val[0]);
	voltage = (float)(ad_val[0])/1024.0 * 3.0 * 2.0;

}

/**
 * debug用関数
 *
 * @param void
 * @return void
 */
void debugUI(void)
{
	myPrintfUSB("#######debug UI####### \n");
	myPrintfUSB("RSW state %d \n", getStateSW());
	myPrintfUSB("RSW count %d msec \n", getSWcount());
	myPrintfUSB("Elap Time %d msec \n", getElapsedTime());
	myPrintfUSB("Battery V %f V \n", getVoltage());
}

/******************************************************************************
**                            End Of File
******************************************************************************/
