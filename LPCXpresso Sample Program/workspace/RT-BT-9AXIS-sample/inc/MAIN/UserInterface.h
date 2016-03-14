/**
 * @file   UserInterface.h
 * @brief  LED,SW,電源電圧の監視等の機能の実装 <br>
 *
 * @author RTCorp. Ryota Takahashi
 */

#ifndef USERINTERFACE_H
#define USERINTERFACE_H

void initUI(void);

uint8_t  getStateSW(void);
uint16_t getSWcount(void);

uint32_t getElapsedTime(void);
void resetElapsedTime(void);

void turnLED(uint8_t state);

void flashLED(uint16_t on_count, uint16_t off_count);

float getVoltage(void);

void debugUI(void);

#endif

/******************************************************************************
**                            End Of File
******************************************************************************/
