/* ------------------------------------------------------------ *
File ad.c

adポート関係
* ------------------------------------------------------------ */
#include "timer.h"
#include "LPC13xx.h"			// LPC13xx Peripheral Registers

/*----------------------------------------------------------------------------
  Input  void
  Output void
  ad変換関係の設定
 *---------------------------------------------------------------------------*/
void Init_ad (void)
{
	LPC_SYSCON->PDRUNCFG &=  ~(0x1<<4);     // ADCのpower-downの設定 4bit目: 0:Powered 1:Power down
	LPC_SYSCON->SYSAHBCLKCTRL |= (0x1<<13); // ACCのクロックのenable 13bit目: 0:disabled 1: enabled


	//PIO0_11 AD AD0
	LPC_IOCON->R_PIO0_11 |=  ( 0x1<<1 );             // Selects function AD0
	LPC_IOCON->R_PIO0_11 &= ~( (0x1<<3)|(0x1<<4) );  // pull up pull down off
	LPC_IOCON->R_PIO0_11 &= ~( 0x01<<7 );            // 7bit目 ADMODE 0:Analog input mode 1:Digital gunction mode
#ifdef COMMENT_OUT
	//PIO1_0  AD AD1
	LPC_IOCON->R_PIO1_0 |=  ( 0x1<<1 );             // Selects function AD1
	LPC_IOCON->R_PIO1_0 &= ~( (0x1<<3)|(0x1<<4) );  // pull up pull down off
	LPC_IOCON->R_PIO1_0 &= ~( 0x01<<7 );            // 7bit目 ADMODE 0:Analog input mode 1:Digital gunction mode

	//PIO1-1  AD2
	LPC_IOCON->R_PIO1_1 |=  ( 0x1<<1 );             // Selects function AD2
	LPC_IOCON->R_PIO1_1 &= ~( (0x1<<3)|(0x1<<4) );  // pull up pull down off
	LPC_IOCON->R_PIO1_1 &= ~( 0x01<<7 );            // 7bit目 ADMODE 0:Analog input mode 1:Digital gunction mode

	//PIO1-2  AD3
	LPC_IOCON->R_PIO1_2 |=  ( 0x1<<1 );             // Selects function AD3
	LPC_IOCON->R_PIO1_2 &= ~( (0x1<<3)|(0x1<<4) );  // pull up pull down off
	LPC_IOCON->R_PIO1_2 &= ~( 0x01<<7 );            // 7bit目 ADMODE 0:Analog input mode 1:Digital gunction mode

	//PIO1-4  AD5
	LPC_IOCON->PIO1_4 |= 0x1;                       // Selects function AD5
	LPC_IOCON->PIO1_4 &= ~( (0x1<<3)|(0x1<<4) );    // pull up pull down off
	LPC_IOCON->PIO1_4 &= ~( 0x01<<7 );              // 7bit目 ADMODE 0:Analog input mode 1:Digital gunction mode

	//PIO1-10 AD6
	LPC_IOCON->PIO1_10 |= 0x1;                      // Selects function AD6
	LPC_IOCON->PIO1_10 &= ~( (0x1<<3)|(0x1<<4) );   // pull up pull down off
	LPC_IOCON->PIO1_10 &= ~( 0x01<<7 );             // 7bit目 ADMODE 0:Analog input mode 1:Digital gunction mode
#endif
	//8-15bit目 CLKDIV
	LPC_ADC->CR |= ((SystemCoreClock/LPC_SYSCON->SYSAHBCLKDIV)/4000000-1)<<8;



}

/*----------------------------------------------------------------------------
  Input  ad_value[8]
  Output void
  ad値の取得
  ad_value[0] DR0
  ad_value[1] DR1
  ad_value[2] DR2
  ad_value[3] DR3
  ad_value[4] なし
  ad_value[5] DR5
  ad_value[6] DR6
  ad_value[7] なし
 *---------------------------------------------------------------------------*/
void storeAD2Array( int16_t *ad_value )
{

	//DR0
	LPC_ADC->CR &=0xffffff00;
	LPC_ADC->CR |=  (0x1<<0);              //0-7bit目 SEL
	LPC_ADC->CR |=0x01000000;              //ADC Start now
	while(!(LPC_ADC->DR0 & 0x80000000 ));  //各ピンのAD変換が終了するまで待つ.
	ad_value[0]=(LPC_ADC->DR0 & 0x0000FFC0)>>6;
	LPC_ADC->CR &= 0xF8FFFFFF;;	//stop ADC now
#ifdef COMMENT_OUT
	//DR1
	LPC_ADC->CR &=0xffffff00;
	LPC_ADC->CR |=  (0x1<<1);
	LPC_ADC->CR |=0x01000000;              //ADC Start now
	while(!(LPC_ADC->DR1 & 0x80000000 ));
	ad_value[1]=(LPC_ADC->DR1 & 0x0000FFC0)>>6;
	LPC_ADC->CR &= 0xF8FFFFFF;;	//stop ADC now
	//DR2
	LPC_ADC->CR &=0xffffff00;
	LPC_ADC->CR |=  (0x1<<2);
	LPC_ADC->CR |=0x01000000;              //ADC Start now
	while(!(LPC_ADC->DR2 & 0x80000000 ));
	ad_value[2]=(LPC_ADC->DR2 & 0x0000FFC0)>>6;
	LPC_ADC->CR &= 0xF8FFFFFF;;	//stop ADC now
	//DR3
	LPC_ADC->CR &=0xffffff00;
	LPC_ADC->CR |=  (0x1<<3);
	LPC_ADC->CR |=0x01000000;              //ADC Start now
	while(!(LPC_ADC->DR3 & 0x80000000 ));
	ad_value[3]=(LPC_ADC->DR3 & 0x0000FFC0)>>6;
	LPC_ADC->CR &= 0xF8FFFFFF;;	//stop ADC now
	//DR5
	LPC_ADC->CR &=0xffffff00;
	LPC_ADC->CR |=  (0x1<<5);
	LPC_ADC->CR |=0x01000000;              //ADC Start now
	while(!(LPC_ADC->DR5 & 0x80000000 ));
	ad_value[5]=(LPC_ADC->DR5 & 0x0000FFC0)>>6;
	LPC_ADC->CR &= 0xF8FFFFFF;;	//stop ADC now
	//DR6
	LPC_ADC->CR &=0xffffff00;
	LPC_ADC->CR |=  (0x1<<6);
	LPC_ADC->CR |=0x01000000;              //ADC Start now
	while(!(LPC_ADC->DR6 & 0x80000000 ));
	ad_value[6]=(LPC_ADC->DR6 & 0x0000FFC0)>>6;
	LPC_ADC->CR &= 0xF8FFFFFF;;	//stop ADC now
#endif

}


/******************************************************************************
**                            End Of File
******************************************************************************/
