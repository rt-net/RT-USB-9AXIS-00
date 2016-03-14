/* ------------------------------------------------------------ *
File usbTransmission.c

USBでの通信用関数群
* ------------------------------------------------------------ */
#include "type.h"
#include "LPC13xx.h"
#include "usbTransmission.h"

//USB,CDC関連
#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "cdc.h"
#include "cdcuser.h"

//時間待ちタイマー
#include "SystemTickTimer.h"
#include "debug.h"
volatile static uint8_t flag_usb_config = 0; //USBの接続確認用フラグ 1:USB接続は確立済み 0:USB接続は未確立


/*----------------------------------------------------------------------------
  Input  void
  Output void

  USBからデータを送る
 *---------------------------------------------------------------------------*/
void VCOM_SendData(GETDAT_T dat)
{
	if (dat.num!=0 && CDC_DepInEmpty)
	{ // 1 is empty so if empty
		CDC_DepInEmpty = 0;
		USB_WriteEP (CDC_DEP_IN, (unsigned char* )(&dat.d[0]), dat.num);
	}
}
/*----------------------------------------------------------------------------
  Input  void
  Output void

  VCOM_SendData()を呼んだ直後に呼ぶこと
 *---------------------------------------------------------------------------*/
void VCOM_CheckState (void)
{
	unsigned short temp;
	volatile static unsigned short serialState;

	temp = CDC_GetSerialState();
	if (serialState != temp)
	{
		serialState = temp;
		CDC_NotificationIn(); // send SERIAL_STATE notification
	}
}
/*----------------------------------------------------------------------------
  Input  void
  Output 0:USBの接続を試みたがタイムアウト  1:USBの接続が確立

  USBのコンフィギュレーションが完了したら1を返す
  usbTransmisson.h内のUSB_TIMEOUTでタイムアウト時間を設定
  USBのコンフィギュレーションがUSB_TIMEOUTmsec以内終わらなければ
  0を返す.
 *---------------------------------------------------------------------------*/
uint8_t tryUSBConnect(void)
{
	volatile uint16_t count = 0;
	//PIO0_3ピンの状態 H:USBコネクタが接続されている L:USBコネクタが未接続
	//USBコネクタが繋がれている場合のみUSB接続を試みる
	if( ((LPC_GPIO0->DATA & 0x8)>>3))
	{
		USB_Connect(1);
	    while (!USB_Configuration){
	    	wait1msec(1);
	    	count ++;
	    	if(count > 6000){
	    		flag_usb_config = 0;
	    		return 0;
	    	};
	    };
		flag_usb_config = 1;
	    return 1;
	}
	else
	{
		//USBコネクタが繋がっていないのでPIO0_6ピンをHIGHにする
		LPC_GPIO0->DATA |= 0x0040;
		flag_usb_config = 0;
		return 0;
	}
}
/*----------------------------------------------------------------------------
  Input  void
  Output 0:USBの接続が未確立  1:USBの接続が確立

　　USBの接続が確立しているかを返す
 *---------------------------------------------------------------------------*/
uint8_t isUSBConfig(void)
{
	return flag_usb_config;
}


/******************************************************************************
**                            End Of File
******************************************************************************/
