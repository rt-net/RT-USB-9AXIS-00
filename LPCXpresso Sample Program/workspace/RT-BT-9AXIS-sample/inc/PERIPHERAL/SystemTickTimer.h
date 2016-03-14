/* ------------------------------------------------------------ *
File SystemTickTimer.h

SystemTickTimerを用いた関数群
* ------------------------------------------------------------ */
#ifndef SYSTEMTICKTIMER_H
#define STSTEMTICKTIMER_H

//1msec,1usec刻みのタイマー
void wait1usec(uint32_t wait_count);
void wait1msec(uint32_t wait_count);

//1usec刻みの時間測定用関数
void time_count_start(void);
uint32_t time_count_end(void);

#endif


/******************************************************************************
**                            End Of File
******************************************************************************/
