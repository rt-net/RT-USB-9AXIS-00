/* ------------------------------------------------------------ *
File MainFunction.h

* ------------------------------------------------------------ */
#ifndef MAINFUNCTION_H
#define MAINFUNCTION_H

void startTimerInterruptMainFunction(void);
void stopTimerInterruptMainFunction(void);
void setSendDataEnable(uint8_t en);

void writeEEPROMOutputMode(uint8_t mode);
uint8_t readEEPROMOutputMode(void);
void setOutputMode(uint8_t mode);

//出力データの形式
#define OUTPUT_MODE_57600_BINARY_9AXIS (0)
#define OUTPUT_MODE_230400_ASCII_9AXIS (1)
#define OUTPUT_MODE_230400_BINARY_9AXIS_QUAT_EULER (2)
#define OUTPUT_MODE_230400_ASCII_9AXIS_QUAT  (3)
#define OUTPUT_MODE_230400_ASCII_9AXIS_EULER (4)

#endif

/******************************************************************************
**                            End Of File
******************************************************************************/
