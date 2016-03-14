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
volatile static uint8_t flag_flash_green_LED = 0;
volatile static uint8_t flag_flash_red_LED = 0;

//SWの押下状態のカウント
volatile static uint16_t SW_count = 0;
//LEDの点滅用カウント
volatile static uint16_t green_LED_count   = 0;
volatile static uint16_t red_LED_count   = 0;
//経過時間のカウント
volatile static uint32_t elapsed_time_count = 0;
//LEDの点滅時間のon/off時間のカウント
volatile static uint16_t on_count_green_LED    = 0;
volatile static uint16_t off_count_green_LED   = 1;

volatile static uint16_t on_count_red_LED    = 0;
volatile static uint16_t off_count_red_LED   = 1;


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
 * スイッチの押下状態取得
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
 *  緑LEDの点灯と消灯
 *
 * @param  state 1:点灯 0:消灯
 * @return void
 */
static void setStateGreenLED(uint8_t state)
{
	if(state == 1)
	{
		LPC_GPIO1->DATA |= 0x0004; //LED on
	}
	else
	{
		LPC_GPIO1->DATA &= ~0x0004; //LED off
	}
}

/**
 *  赤LEDの点灯と消灯
 *
 * @param  state 1:点灯 0:消灯
 * @return void
 */
static void setStateRedLED(uint8_t state)
{
	if(state == 1)
	{
		LPC_GPIO1->DATA |= 0x0002; //LED on
	}
	else
	{
		LPC_GPIO1->DATA &= ~0x0002; //LED off
	}
}


/**
 * 緑LEDの点灯と消灯.  緑LEDが点滅状態で
 * 呼んだ場合は常時点灯(消灯)状態になる.
 *
 * @param  state 1:点灯 0:消灯
 * @return void
 */
void turnGreenLED(uint8_t state)
{
	flag_flash_green_LED = 0;
	setStateGreenLED(state);
}

/**
 * 赤LEDの点灯と消灯.  赤LEDが点滅状態で
 * 呼んだ場合は常時点灯(消灯)状態になる.
 *
 * @param  state 1:点灯 0:消灯
 * @return void
 */
void turnRedLED(uint8_t state)
{
	flag_flash_red_LED = 0;
	setStateRedLED(state);
}


/**
 * 緑LEDを指定した時間間隔で点滅させる <br>
 * 例.緑LEDを100msecの間on, 50msecの間offで点滅させるには <br>
 *    on_count = 100, off_count = 50 <br>
 * 緑LEDを常時点灯する場合は <br>
 *    turnGreenLED(1) <br>
 * 緑LEDを消灯する場合は <br>
 *    turnGreenLED(0) <br>
 *
 * @param  on_count  [msec]
 * @param  off_count [msec]
 * @return void
 */
void flashGreenLED(uint16_t on_count, uint16_t off_count)
{
	flag_flash_green_LED = 1;
	on_count_green_LED    = on_count;
	off_count_green_LED   = off_count;
}

/**
 * 赤LEDを指定した時間間隔で点滅させる <br>
 * 例.赤LEDを100msecの間on, 50msecの間offで点滅させるには <br>
 *    on_count = 100, off_count = 50 <br>
 * 赤LEDを常時点灯する場合は <br>
 *    turnRedLED(1) <br>
 * 赤LEDを消灯する場合は <br>
 *    turnRedLED(0) <br>
 *
 * @param  on_count  [msec]
 * @param  off_count [msec]
 * @return void
 */
void flashRedLED(uint16_t on_count, uint16_t off_count)
{
	flag_flash_red_LED = 1;
	on_count_red_LED    = on_count;
	off_count_red_LED   = off_count;
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
	LPC_TMR32B0->IR=0x08; //clear interrupt flag

	//緑LEDの点滅処理
	if(flag_flash_green_LED == 1)
	{
		//LEDの点滅
		if(green_LED_count < on_count_green_LED ) setStateGreenLED(1);
		else setStateGreenLED(0);
		if( (on_count_green_LED + off_count_green_LED) < green_LED_count ) green_LED_count = 0;
		green_LED_count ++;
	}

	//赤LEDの点滅処理
	if(flag_flash_red_LED == 1)
	{
		//LEDの点滅
		if(red_LED_count < on_count_red_LED ) setStateRedLED(1);
		else setStateRedLED(0);
		if( (on_count_red_LED + off_count_red_LED) < red_LED_count ) red_LED_count = 0;
		red_LED_count ++;
	}

	//時間経過
	elapsed_time_count ++;

	//スイッチの押下
	if(getStateSW() == 1) SW_count ++;
	else SW_count = 0;

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
	myPrintfUSB("SW state %d \n", getStateSW());
	myPrintfUSB("SW count %d msec \n", getSWcount());
	myPrintfUSB("Elap Time %d msec \n", getElapsedTime());
}

/******************************************************************************
**                            End Of File
******************************************************************************/
