/* ------------------------------------------------------------ *
File SystemTickTimer.c

SystemTickTimerを用いた関数群
1msec,1usec刻みのタイマー
1usecの時間測定用関数
* ------------------------------------------------------------ */
#include "type.h"
#include "LPC13xx.h"			// LPC13xx Peripheral Registers
#include "core_cm3.h"
#include "SystemTickTimer.h"

volatile static uint32_t systick_wait_count;
volatile static uint32_t systick_time_count;
volatile static uint8_t systick_handler_state;


/*----------------------------------------------------------------------------
  Input  void
  Output void

  SysTickTimerの初期化
 *---------------------------------------------------------------------------*/
static void initSystemTickTimer()
{
	SysTick->CTRL  = 0;
	SysTick->LOAD  = 0;
	SysTick->VAL   = 0;
	SysTick->CTRL  = 0x7;
	SysTick->LOAD  = 72-1;  //720000-1で10msec, 72-1で1usec
	systick_handler_state = 1;
};
/*----------------------------------------------------------------------------
  Input  wait_count
  Output void

  wait_count * 1usec待つ
    割り込み関数内とmainで同時に呼ばないこと
 *---------------------------------------------------------------------------*/
void wait1usec(uint32_t wait_count)
{
	initSystemTickTimer();
	systick_wait_count = 0;
	while(systick_wait_count < wait_count);
	SysTick->CTRL  = 0x0;
	systick_handler_state = 0;

	return;
};
/*----------------------------------------------------------------------------
  Input  wait_count
  Output void

  wait_count * 1msec待つ
    割り込み関数内とmainで同時に呼ばないこと
 *---------------------------------------------------------------------------*/
void wait1msec(uint32_t wait_count)
{
	if(systick_handler_state == 0)initSystemTickTimer();
	systick_wait_count = 0;
	while(systick_wait_count < wait_count*1000);
	SysTick->CTRL  = 0x0;
	systick_handler_state = 0;
	
	return;
};
/*----------------------------------------------------------------------------
  Input  void
  Output void

  time_count_end(void)とセットで使用.
  time_count_start()を呼んでから, time_count_end();
    を呼ぶまでsystick_wait_countをインクリメントする.
    時間の測定に使用
 *---------------------------------------------------------------------------*/
void time_count_start(void)
{
	if(systick_handler_state == 0)initSystemTickTimer();
	systick_time_count = 0;

}
/*----------------------------------------------------------------------------
  Input  void
  Output systick_wait_count タイマのカウント値

  time_count_start(void)とセットで使用.
 *---------------------------------------------------------------------------*/
uint32_t time_count_end(void)
{
	SysTick->CTRL  = 0x0;
	systick_handler_state = 0;
	return systick_time_count;
}
/*----------------------------------------------------------------------------
  Input  void
  Output void
  SysTickTimerの割り込み関数.
 *---------------------------------------------------------------------------*/
void SysTick_Handler (void)
{
	systick_wait_count ++;
	systick_time_count ++;
}


/******************************************************************************
**                            End Of File
******************************************************************************/
