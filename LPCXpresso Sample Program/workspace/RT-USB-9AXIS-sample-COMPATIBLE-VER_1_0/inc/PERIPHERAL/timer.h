/* ------------------------------------------------------------ *
File timer.h

32bitタイマーの初期化関数群
タイマ割り込みの割り込み関数はmain.cに記述
* ------------------------------------------------------------ */
#ifndef TIMER_H
#define TIMER_H

void initTimer32_0(void);
void startTimer32_0(void);
void stopTimer32_0(void);

void initTimer32_1(void);
void startTimer32_1(void);
void stopTimer32_1(void);

#endif


/******************************************************************************
**                            End Of File
******************************************************************************/
