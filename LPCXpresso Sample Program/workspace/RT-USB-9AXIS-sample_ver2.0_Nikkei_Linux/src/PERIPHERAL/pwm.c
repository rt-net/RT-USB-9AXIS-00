/* ------------------------------------------------------------ *
 File pwm.c

16bitタイマーを用いたPWM出力の設定
* ------------------------------------------------------------ */
#include "timer.h"
#include "LPC13xx.h"			// LPC13xx Peripheral Registers
#include "pwm.h"

/*----------------------------------------------------------------------------
  Input  void
  Output void
  PIO0_10/CT16B0_MAT2ピンを16bitタイマーを利用したPWM出力に設定
 *---------------------------------------------------------------------------*/
void Init_PWM1 (void)
{
	LPC_TMR16B0->TCR = 0; //Disable Timer0

	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<7);  // Timer16B0 Turn ON
	LPC_IOCON->SWCLK_PIO0_10 = 0x3;	// PIO0_10 as CT16B0_MAT2

	LPC_TMR16B0->PR   = 72-1;// TC周波数決定　72Mhz/720 = 100kHz
	LPC_TMR16B0->PWMC = 0x4;

	LPC_TMR16B0->MCR = (1<<10);
	LPC_TMR16B0->MR3 = 5000;	// PWM High duration=MR2-MR0
	LPC_TMR16B0->MR2 = 1;	// PWM Period (Max 16bit dec:65535)

	LPC_TMR16B0->TCR = 2;	// TCR Reset
	LPC_TMR16B0->TCR = 1;	// TCR Start

}

/*----------------------------------------------------------------------------
  Input  void
  Output void

  PIO1_9/CT16B1_MAT0ピンを16bitタイマーを利用したPWM出力に設定
 *---------------------------------------------------------------------------*/
void Init_PWM2 (void)
{
	LPC_TMR16B1->TCR = 0; //Disable Timer1

	LPC_SYSCON->SYSAHBCLKCTRL |=0x100; // Timer16B1 Turn ON
	LPC_IOCON->PIO1_9 = 0x001;	// PIO1_9 as CT16B1_MAT0

	LPC_TMR16B1->PR = 72-1;	// TC周波数決定　72Mhz/720 = 100kHz
	LPC_TMR16B1->PWMC = 1;	// MAT0 as PWM

	LPC_TMR16B1->MCR = 0x80;
	LPC_TMR16B1->MR0 = 0;
	LPC_TMR16B1->MR2 = 5000;

	LPC_TMR16B1->TCR = 2;	// TCR Reset
	LPC_TMR16B1->TCR = 1;	// TCR Start
}

/*----------------------------------------------------------------------------
  Input  duty  0.0 to　1.0
  Output void
  PWMのdutyを duty*100 %　に設定
 *---------------------------------------------------------------------------*/
void setDutyPWM1 (float duty)
{
	LPC_TMR16B0->MR2 =  (uint32_t)(5000.0* duty);
}
/*----------------------------------------------------------------------------
  Input  duty 0.0 to 1.0
  Output void
  PWMのdutyを duty*100 %　に設定
 *---------------------------------------------------------------------------*/
void setDutyPWM2 (float duty)
{
	LPC_TMR16B1->MR0 =  (uint32_t)(5000.0* duty);
}

/******************************************************************************
**                            End Of File
******************************************************************************/
