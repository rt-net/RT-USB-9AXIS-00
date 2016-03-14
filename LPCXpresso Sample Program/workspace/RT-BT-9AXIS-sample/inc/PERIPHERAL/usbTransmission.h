/* ------------------------------------------------------------ *
File usbTransmission.h

USBでの通信用関数群
* ------------------------------------------------------------ */
#ifndef USBTRANSMISSION_H
#define USBTRANSMISSION_H

//送信データ格納用構造体
typedef struct {
	uint8_t d[100];
	int num;
}GETDAT_T;

void VCOM_SendData(GETDAT_T dat);
void VCOM_CheckState (void);
uint8_t tryUSBConnect(void);
uint8_t isUSBConfig(void);

#endif


/******************************************************************************
**                            End Of File
******************************************************************************/
