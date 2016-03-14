/* ------------------------------------------------------------ *
File debug.c

デバッグ用関数群
* ------------------------------------------------------------ */

#include <stdarg.h>
#include <stdio.h>

#include "LPC13xx.h"
#include "core_cm3.h"
#include "type.h"

#include "uart.h"
#include "usbTransmission.h"

/* ------------------------------------------------------------ *
  Input  *fmt, ... printf()と同じフォーマット
  Output dat.num データ長

  printf()のフォーマットでUSBから文字列を出力
* ------------------------------------------------------------ */
int myPrintfUSB(const char *fmt, ...){
	GETDAT_T dat;

    va_list ap;
    va_start(ap, fmt);

    dat.num = vsprintf(&dat.d[0], fmt, ap);
    VCOM_SendData(dat);
    VCOM_CheckState ();

    va_end(ap);
    return dat.num;
}
/* ------------------------------------------------------------ *
  Input  *fmt, ...  printf()と同じフォーマット
  Output dat.num データ長

  printf()のフォーマットでUARTで文字列を出力
* ------------------------------------------------------------ */
int myPrintfUART(const char *fmt, ...){
    static char buffer[100];
    int len;

    va_list ap;
    va_start(ap, fmt);

    len = vsprintf(buffer, fmt, ap);

    UARTSend(buffer, len );

    va_end(ap);
    return len;
}



/******************************************************************************
**                            End Of File
******************************************************************************/
