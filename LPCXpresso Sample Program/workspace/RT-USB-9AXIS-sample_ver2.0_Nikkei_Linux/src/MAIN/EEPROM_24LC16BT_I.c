/**
 * @file   24LC16BT-I.c
 * @brief  16k bit = 2kbyte = (256 x 8) x 8 bitのeepromである24LC16BT-Iに関する記述<br>
 *         1ブロックは (8bitの領域　x 256個).  このeepromにはブロックが8個ある.<br>
 *         このeepromは16byteのpage writeが可能	<br>
 *         page writeにかかる時間は5ms<br>
 *         i2cのクロックは100kHzから400kHzまで可能<br>
 *         データ格納先アドレスの指定は 0-7までのブロック番号と0-255までの番地の二つ.<br>
 *         ブロックをまたがったデータの読み書きはできない<br>
 *
 * @author RTCorp. Ryota Takahashi
 */

#include "LPC13xx.h"
#include "core_cm3.h"
#include "type.h"
#include "EEPROM_24LC16BT_I.h"
#include "i2c.h"
#include "SystemTickTimer.h"
#include "debug.h"


//i2cの操作に必要なもの達のextern宣言.  実体はi2c.cにある
extern volatile uint8_t  I2CMasterBuffer[BUFSIZE];
extern volatile uint8_t  I2CSlaveBuffer[BUFSIZE];
extern volatile uint32_t I2CReadLength, I2CWriteLength;

/**
 * ブロックとアドレスを指定してEEPROMに1byteデータを書き込む.
 * 一回の書き込みに5msec必要
 *
 * @param block 書き込むブロックの指定 0から7まで
 * @param adress アドレスの指定 0から255まで
 * @param write_data 書き込むデータ
 */
void write1byteEEPROM(uint8_t block, uint8_t address, uint8_t write_data){
	I2CWriteLength = 3;
	I2CReadLength = 0;
	uint8_t control_byte = 0xa0 | (block<<1) | 0x0;

	I2CMasterBuffer[0] = control_byte;
	I2CMasterBuffer[1] = address;
	I2CMasterBuffer[2] = write_data;
	I2CEngine();
	wait1msec(5);
}

/**
 * 読み出すデータのブロックとアドレスを指定してEEPROMから1byteデータを読みこむ
 *
 * @param block 書き込むブロックの指定 0から7まで
 * @param adress アドレスの指定 0から255まで
 */
uint8_t read1byteEEPROM(uint8_t block,uint8_t address)
{
	uint8_t read_data;
	uint16_t i;
	uint8_t control_byte = 0xa0 | (block<<1) | 0x0;

	I2CWriteLength = 2;
	I2CReadLength = 1;
	I2CMasterBuffer[0] = control_byte;
	I2CMasterBuffer[1] = address;
	control_byte = 0xa0 | (block<<1) | 0x1;
	I2CMasterBuffer[2] = control_byte;
	I2CEngine();

	read_data = I2CSlaveBuffer[0];

	for ( i = 0; i < BUFSIZE; i++ ) //clear I2CSlaveBuffer
	{
		I2CSlaveBuffer[i] = 0x00;
	}

	return read_data;
}

/**
 * ブロックとアドレスを指定してEEPROMに指定byteデータを読みこむ  <br>
 * この関数では1byte書き込みを逐一繰り返しNbyte書き込みを実現 <br>
 * ブロックをまたがった書き込みはできないので注意 <br>
 * Nbyteの書き込みにN*5msecかかる
 * start_adressから連続lengthbyteにdata配列を格納
 *
 * @param block 書き込むブロックの指定 0から7まで
 * @param start_adress アドレスの指定 0から255まで
 * @param *data 書き込むデータの配列
 */
void writeNbyteEEPROM(uint8_t block, uint8_t start_address, uint8_t *data,  uint16_t length)
{
	uint8_t i = 0;
	for(i=0; i< length; i++){
		write1byteEEPROM(block,start_address+i,data[i]);
	}
}

/**
 * 読み出すデータのブロックとアドレスを指定してEEPROMから指定byteデータを読み込む<br>
 * ブロックをまたがった書き込みはできないので注意<br>
 *
 * @param block 読み込むブロックの指定 0から7まで
 * @param start_adress アドレスの指定 0から255まで
 * @param *data  読み出したデータの格納先の配列
 * @param length 読み出すデータの長さ
 */
void readNbyteEEPROM(uint8_t block, uint8_t start_address, uint8_t *data, uint16_t length)
{
	uint8_t i = 0;
	uint8_t control_byte = 0xa0 | (block<<1) | 0x0;

	I2CWriteLength = 2;
	I2CReadLength = length;
	I2CMasterBuffer[0] = control_byte;
	I2CMasterBuffer[1] = start_address;
	control_byte = 0xa0 | (block<<1) | 0x1;
	I2CMasterBuffer[2] = control_byte;
	I2CEngine();
	for(i = 0; i < length; i++){
		data[i] = I2CSlaveBuffer[i];
	}
}

/**
 * 指定したブロックのデータをすべて表示する.
 *
 * @param block 読み込むブロックの指定 0から7まで
 */
void printBlockData(uint8_t block){
	uint16_t i;
	uint8_t read_data;
	myPrintfUSB("====BLOCK %d====\n",block);
	for(i=0;i<256;i++)
	{
		wait1msec(3);
		read_data = read1byteEEPROM(block,i);
		myPrintfUSB("%d| %d %x\n", i,read_data,read_data);
		wait1msec(3);
	}
}

/**
 * EEPROM内のすべてのデータを表示する.
 */
void printAllData()
{
	uint16_t i;
	for(i=0;i<8;i++)
	{
		printBlockData(i);
	}
}

