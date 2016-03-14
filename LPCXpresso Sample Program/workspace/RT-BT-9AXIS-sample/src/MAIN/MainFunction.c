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

#include "mpu9250.h"
#include "debug.h"
#include "uart.h"
#include "tinyMathFunction.h"

#include "usbTransmission.h"


///////uart.cに宣言があり/////////////////////////
extern volatile uint32_t UARTCount;
extern volatile uint8_t  UARTBuffer[BUFSIZE];
//////////////////////////////////////////////

volatile static uint8_t flag_send_data_enable = 0;

volatile static uint32_t time;

/**
 * AICHIPFunction.c内の関数を使用できるようにする
 *
 * @param void
 * @return void
 */
void initMainFunction(void)
{
	startTimer32_1();
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
 * 10msec毎に行われる処理 <br>
 *          <li>各種情報をUART,USBで送信 </li>
 *         </ul>
 * @param  void
 * @return void
 */
void TIMER32_1_IRQHandler(void)
{

	static uint16_t count = 0;
	GETDAT_T send_data;

	static uint8_t timestamp;

	float acc_vec[3];
	float omega_vec[3];
	float omega_str_LPF_vec[3]; //強いローパスフィルタがかかった値を格納

    uint16_t v_bat;


	LPC_TMR32B1->IR=0x08; //clear interrupt flag

	//送信データ格納用変数のヘッダ
	send_data.num  = 30;
	send_data.d[0] = 0xff;
	send_data.d[1] = 0xff;
	send_data.d[2] = 0x52;
	send_data.d[3] = 0x54;
	send_data.d[4] = 0x39;
	send_data.d[5] = 0x43;
	send_data.d[6] = 0x02;  // 10/20 version
	send_data.d[7] = timestamp;

	if(timestamp == 0xff) timestamp = 0;
	else timestamp++;

	//9軸センサの観測値を変数に格納
	getOmega(&omega_vec[0]);
	getAccLPF(&acc_vec[0]);
	getOmegaStrLPF(&omega_str_LPF_vec[0]);

	//Batteryの電圧を送信用配列に格納
	v_bat  = (uint16_t)(getVoltage() * 13107);  //0.0~5.0を0から65535に変換
	send_data.d[28] =  0x000000ff & v_bat;
	send_data.d[29] = (0x0000ff00 & v_bat)>>8;

	//9軸センサの観測値を送信用配列に格納
	getDataFromMPU9250( (uint8_t *)(&send_data.d[8]) );
	getDataFromAK8963( (uint8_t *)(&send_data.d[22]) );

	//20Hzの周期でデータを送る (割り込み周期100Hz 5回に一回)
	if(count % 5 == 0 )
	{
			//UART経由でデータを送信
		LPC_UART->IER = IER_THRE | IER_RLS;			    // Disable RBR
		UARTSend(&send_data.d[0], send_data.num);
		LPC_UART->IER = IER_THRE | IER_RLS | IER_RBR;	// Re-enable RBR

	}

	if(isUSBConfig() == 1 && flag_send_data_enable == 1 ) //USB接続確立
	{
		//USB経由でデータを送信
		VCOM_SendData(send_data);
	}

	count++;

}






/******************************************************************************
**                            End Of File
******************************************************************************/
