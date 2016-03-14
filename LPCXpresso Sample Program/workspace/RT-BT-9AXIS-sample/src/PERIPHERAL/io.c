/* ------------------------------------------------------------ *
File io.c

IOポートの設定
* ------------------------------------------------------------ */
#include "io.h"
#include "LPC13xx.h"
#include "core_cm3.h"
#include "type.h"

/* ------------------------------------------------------------ *
  Input  void
  Output void

  IOポートの設定.
  SW1,LED1に繋がっているIOピンを設定
* ------------------------------------------------------------ */
void initIO (void)
{

	//SW1
	LPC_IOCON->PIO0_1 = 0x0010;   //IOConfig PIO0_1
	LPC_GPIO0->DIR &= 0xfffd;     //Data Direction(1 output , 0 input)   [1bit] SW1

	//LED1
	LPC_IOCON->PIO2_0 = 0x0000;   //IOConfig PIO2_0
	LPC_GPIO2->DIR |= 0x0001;     //Data Direction(1 output , 0 input)   [0bit] LED1

};
/******************************************************************************
**                            End Of File
******************************************************************************/
