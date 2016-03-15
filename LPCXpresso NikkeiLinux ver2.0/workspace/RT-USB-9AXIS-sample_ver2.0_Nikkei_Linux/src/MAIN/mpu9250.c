/**
 * @file   mpu9250.c
 * @brief  MPU9250を使えるようにするための関数群 <br>
 *         注1)mpu9150とmpu9250では地磁気センサの出力分解能が異なる <br>
 *            本プログラムはmpu9250用だがVER_1_0_COMPATIBLE_MPU9150_MAG<br>
 *            をdefineするかどうかで出力の形式を選択可能<br>
 *         注2)mpu9150とmpu9250では温度センサの出力を変換する際の式が微妙に異なる<br>
 *            本プログラムはmpu9250用だがVER_1_0_COMPATIBLE_MPU9250_TEMP<br>
 *            をdefineするかどうかで出力の形式をmpu9150を用いた際と互換することが可能
 *
 * @author RTCorp. Ryota Takahashi
 */

//#define VER_1_0_COMPATIBLE_MPU9150_MAG
//#define VER_1_0_COMPATIBLE_MPU9150_TEMP

#include "type.h"
#include "mpu9250.h"
#include "i2c.h"
#include "uart.h"
#include "SystemTickTimer.h"
#include "MainFunction.h"
#include "TinyMathFunction.h"
#include "debug.h"
#include "EEPROM_24LC16BT_I.h"


//i2cの操作に必要なもの達のextern宣言.  実体はi2c.cにある
extern volatile uint8_t  I2CMasterBuffer[BUFSIZE];
extern volatile uint8_t  I2CSlaveBuffer[BUFSIZE];
extern volatile uint32_t I2CReadLength, I2CWriteLength;

//MPU9250のi2cのアドレス
#define MPU9250_W		0xd0
#define MPU9250_R		0xd1
#define AK8963_W		0x18
#define AK8963_R		0x19

static volatile float omega_vec[3]            = {0.0, 0.0, 0.0};   //角速度ベクトル (x,y,z)[rad]
static volatile float acc_vec[3]              = {0.0, 0.0, 0.0};   //加速度ベクトル (x,y,z) [1G]
static volatile float mag_vec[3]              = {0.0, 0.0, 0.0};   //地磁気ベクトル (x,y,z) [uT]
static volatile float temperature             = 0.0;               //MPU9250の温度[°C]

static volatile int16_t omega_vec_int[3]     = {0, 0, 0};   //16bit整数 角速度ベクトル (x,y,z)
static volatile int16_t acc_vec_int[3]       = {0, 0, 0};   //16bit整数 加速度ベクトル (x,y,z)
static volatile int16_t mag_vec_int[3]       = {0, 0, 0};   //16bit整数 地磁気ベクトル (x,y,z)
static volatile int16_t temperature_int      = 0;           //16bit整数 温度

static volatile int16_t omega_vec_raw_int[3] = {0, 0, 0};   //16bit整数 角速度ベクトル生データ (x,y,z)
static volatile int16_t acc_vec_raw_int[3]   = {0, 0, 0};   //16bit整数 加速度ベクトル生データ (x,y,z)
static volatile int16_t mag_vec_raw_int[3]   = {0, 0, 0};   //16bit整数 地磁気ベクトル生データ (x,y,z)
static volatile int16_t temperature_raw_int  = 0;           //16bit整数値 温度生データ

static volatile uint8_t mpu9250ArrayData[14];
static volatile uint8_t ak8963ArrayData[6];

static volatile uint8_t Asa[3]                = {128, 128, 128};   //AK8963の感度補正データ格納用(感度補正データは工場出荷時にICに書き込まれている)
static volatile int16_t omega_offset_vec[3]   = {0,0,0};     //ジャイロのオフセット
static volatile int16_t acc_offset_vec[3]     = {0,0,0};     //加速度のオフセット
static volatile int16_t mag_offset_vec[3]     = {0,0,0};     //地磁気のオフセット

int16_t concatenate2Byte_int(uint8_t H_byte, uint8_t L_byte);

/**
 * MPU9250の初期化(MPU9250のジャイロ,加速度センサ)
 *
 *
 * @param void
 * @return void
 */

void initMPU9250 (void)
{

	I2CWriteLength = 3;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = MPU9250_W;
	I2CMasterBuffer[1] = 0x6B; //Address start MPU9250
	I2CMasterBuffer[2] = 0x00;
	I2CEngine();

	wait1msec(10);

	I2CWriteLength = 3;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = MPU9250_W;
	I2CMasterBuffer[1] = 0x37;  //INT Pin / Bypass Enable Configuration
	I2CMasterBuffer[2] = 0x02;
	I2CEngine();

	wait1msec(10);

	I2CWriteLength = 3;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = MPU9250_W;
	I2CMasterBuffer[1] = 0x1A; //Address digital low pass filter
	I2CMasterBuffer[2] = 0x02; //gyro low pass filter 92Hz
	I2CEngine();

	wait1msec(10);

	I2CWriteLength = 3;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = MPU9250_W;
	I2CMasterBuffer[1] = 0x1B;
	I2CMasterBuffer[2] = 0x18; //2000deg/s
	I2CEngine();

	wait1msec(10);

	I2CWriteLength = 3;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = MPU9250_W;
	I2CMasterBuffer[1] = 0x1C;
	I2CMasterBuffer[2] = 0x18; //16g
	I2CEngine();

	wait1msec(10);

	I2CWriteLength = 3;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = MPU9250_W;
	I2CMasterBuffer[1] = 0x1D;
	I2CMasterBuffer[2] = 0x02; //acc low pass filtter 92Hz
	I2CEngine();

	wait1msec(10);
}

/**
 * Asa[]に感度調整値を格納.
 *
 * @param void
 * @return void
 */
void initAK8963 (void)
{
	volatile uint32_t i;

	I2CWriteLength = 3;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = AK8963_W;
	I2CMasterBuffer[1] = 0x0a; //フューズROMアクセスモードに移行
	I2CMasterBuffer[2] = 0x0f; //
	I2CEngine();

	wait1msec(10);

	I2CWriteLength = 2;
	I2CReadLength = 3;
	I2CMasterBuffer[0] = AK8963_W;
	I2CMasterBuffer[1] = 0x10; //Address 3byte (adjust MagXYZ)
	I2CMasterBuffer[2] = AK8963_R;
	I2CEngine();

	for(i = 0;i < 3;i ++)
	{
		Asa[i] = I2CSlaveBuffer[i];
	}
	wait1msec(10);

	I2CWriteLength = 3;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = AK8963_W;
	I2CMasterBuffer[1] = 0x0a;
	I2CMasterBuffer[2] = 0x00; //パワーダウンモードに変更
	I2CEngine();

	wait1msec(100);

	I2CWriteLength = 3;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = AK8963_W;
	I2CMasterBuffer[1] = 0x0a;
	I2CMasterBuffer[2] = 0x10; //地磁気センサの分解能を16bitに変更
	I2CEngine();

	wait1msec(10);
}

/**
 * MPU9250からデータを読み出し各種変数を更新する <br>
 * 更新する対象は以下 <br>
 * - omega_vec[3]
 * - acc_vec[3]
 * - temperatur
 * - omega_vec_int[3]
 * - acc_vec_int[3]
 * - temperature_int
 * - omega_vec_raw_int[3]
 * - acc_vec_raw_int[3]
 * - temperature_raw_int
 * - mpu9250ArrayData[14]
 */
void updateMPU9250Data (void)
{
	volatile uint32_t i;
	volatile uint16_t acc_temp[3]; //加速度の値計算の中間値
	volatile uint8_t raw_array[14];
	volatile float temperature_temp;


	I2CWriteLength = 2;
	I2CReadLength = 14;
	I2CMasterBuffer[0] = MPU9250_W;
	I2CMasterBuffer[1] = 0x3B; //Address 14byte (AccXYZ + GyroXYZ + temp)
	I2CMasterBuffer[2] = MPU9250_R;
	I2CEngine();

	acc_temp[0] = (I2CSlaveBuffer[0]<<8) + I2CSlaveBuffer[1];
	acc_temp[1] = (I2CSlaveBuffer[2]<<8) + I2CSlaveBuffer[3];
	acc_temp[2] = (I2CSlaveBuffer[4]<<8) + I2CSlaveBuffer[5];

	acc_temp[0] = (~acc_temp[0]) + 1;
	acc_temp[1] = (~acc_temp[1]) + 1;
	acc_temp[2] = (~acc_temp[2]) + 1;

    //Acc
	raw_array[0]  =  0x00ff & acc_temp[0];
	raw_array[1]  = (0xff00 & acc_temp[0])>>8;
	raw_array[2]  =  0x00ff & acc_temp[1];
	raw_array[3]  = (0xff00 & acc_temp[1])>>8;
	raw_array[4]  =  0x00ff & acc_temp[2];
	raw_array[5]  = (0xff00 & acc_temp[2])>>8;
	//temparature
	raw_array[6]  = I2CSlaveBuffer[7];
	raw_array[7]  = I2CSlaveBuffer[6];
	//gyro
	raw_array[8]  = I2CSlaveBuffer[9];
	raw_array[9]  = I2CSlaveBuffer[8];
	raw_array[10] = I2CSlaveBuffer[11];
	raw_array[11] = I2CSlaveBuffer[10];
	raw_array[12] = I2CSlaveBuffer[13];
	raw_array[13] = I2CSlaveBuffer[12];

	for ( i = 0; i < BUFSIZE; i++ ) //clear I2CSlaveBuffer
	{
		I2CSlaveBuffer[i] = 0x00;
	}

	//センサ値を物理量に変換
	for(i=0;i<3;i++)
	{
		//acc
		acc_vec_raw_int[i] = concatenate2Byte_int(raw_array[2*i+1],raw_array[2*i]);
		acc_vec_int[i] = acc_vec_raw_int[i] - acc_offset_vec[i];
		acc_vec[i] = (((float)acc_vec_int[i]))/2048.0;
		//gyro
		omega_vec_raw_int[i] = concatenate2Byte_int(raw_array[2*i+1+8],raw_array[2*i+8]);
		omega_vec_int[i] = omega_vec_raw_int[i] - omega_offset_vec[i];
		omega_vec[i] = DEG2RAD(( ((float)omega_vec_int[i]))/16.4);
	}

	//mpu9150と地磁気の出力の互換性を持たせる場合の記述
	#ifdef VER_1_0_COMPATIBLE_MPU9150_TEMP
	temperature_raw_int = concatenate2Byte_int( raw_array[7],raw_array[6]);
	temperature_temp    = 340.0*((float)(temperature_raw_int)/333.87-14.0);
	temperature_int = (int16_t)(temperature_temp);
	temperature = ((float)temperature_int)/340.0 + 35.0;
	#else
	temperature_raw_int  =  concatenate2Byte_int( raw_array[7],raw_array[6]);
	temperature_int = temperature_raw_int;
	temperature = ((float)temperature_int)/333.87 + 21.0;
	#endif

	for(i=0;i<3;i++){
		mpu9250ArrayData[2*i]   = (0x00ff & acc_vec_int[i]);
		mpu9250ArrayData[2*i+1] = (0xff00 & acc_vec_int[i])>>8;

		mpu9250ArrayData[2*i+8]   = (0x00ff & omega_vec_int[i]);
		mpu9250ArrayData[2*i+1+8] = (0xff00 & omega_vec_int[i])>>8;
	}
	mpu9250ArrayData[6] = (0x00ff & temperature_int);
	mpu9250ArrayData[7] = (0xff00 & temperature_int)>>8;
}

/**
 * MPU9250からデータを読み出し各種変数を更新する <br>
 * 更新する対象は以下 <br>
 * - mag_vec[3]
 * - mag_vec_raw_int[3]
 * - mag_vec_int[3]
 * - ak8963ArrayData[6]
 */
void updateAK8963Data()
{
	volatile uint32_t i;
	volatile int16_t mag_temp[3];
	volatile uint8_t raw_array[6];

	I2CWriteLength = 2;
	I2CReadLength = 6;
	I2CMasterBuffer[0] = AK8963_W;
	I2CMasterBuffer[1] = 0x03; //Address 6byte (MagXYZ)
	I2CMasterBuffer[2] = AK8963_R;
	I2CEngine();

	for ( i = 0; i < 3; i++ )
	{
		mag_temp[i] = concatenate2Byte_int(I2CSlaveBuffer[2*i+1],I2CSlaveBuffer[2*i]);
		mag_temp[i] = (mag_temp[i]*(Asa[i]+128))/256;
	}

	raw_array[0] =  0x00ff & mag_temp[1];
	raw_array[1] = (0xff00 & mag_temp[1])>>8;

	raw_array[2] =  0x00ff & mag_temp[0];
	raw_array[3] = (0xff00 & mag_temp[0])>>8;

	mag_temp[2] = (~mag_temp[2]) + 1; //地磁気センサのZ軸の取り方を下向きから上向きに変更
	raw_array[4] =  0x00ff & mag_temp[2];
	raw_array[5] = (0xff00 & mag_temp[2])>>8;

	I2CWriteLength = 3;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = AK8963_W;
	I2CMasterBuffer[1] = 0x0A; //Address start MagXYZ ADC (need 7.2ms)
	I2CMasterBuffer[2] = 0x11;
	I2CEngine();

	for ( i = 0; i < BUFSIZE; i++ ) //clear I2CSlaveBuffer
	{
		I2CSlaveBuffer[i] = 0x00;
	}

	//センサ値を地磁気ベクトルに変換
	for( i=0;i<3;i++)
	{
		mag_vec_raw_int[i] = concatenate2Byte_int(raw_array[2*i+1],raw_array[2*i]);
		//mpu9150と地磁気の出力の互換性を持たせる場合の記述
		#ifdef VER_1_0_COMPATIBLE_MPU9150_MAG
 		mag_vec_int[i] = (mag_vec_int[i] - mag_offset_vec[i]  )/2;
 		mag_vec[i]  = ((float)mag_vec_int[i])*0.3;
		//mpu9250での記述
 		#else
		mag_vec_raw_int[i] = concatenate2Byte_int(raw_array[2*i+1],raw_array[2*i]);
		mag_vec_int[i] = mag_vec_raw_int[i] - mag_offset_vec[i];
		mag_vec[i]  = ((float)mag_vec_int[i])*0.15;
		#endif
	}

	for(i=0;i<3;i++){
		ak8963ArrayData[2*i]   = (0x00ff & mag_vec_int[i]);
		ak8963ArrayData[2*i+1] = (0xff00 & mag_vec_int[i])>>8;
	}
}

/**
 * MPU9250のデータ (AccXYZ , GyroXYZ , temperature) <br>
 * dest_arrayに次のように格納 <br>
 * byte <br>
 * 0    accX  下位 <br>
 * 1    accX  上位 <br>
 * 2    accY  下位 <br>
 * 3    accY  上位 <br>
 * 4    accZ  下位 <br>
 * 5    accZ  上位 <br>
 * 6    temp  下位 <br>
 * 7    temp  上位 <br>
 * 8    gyroX 下位 <br>
 * 9    gyroX　上位 <br>
 * 10   gyroY　下位 <br>
 * 11   gyroY 上位 <br>
 * 12   gyroZ 下位 <br>
 * 13   gyroZ 上位 <br>
 *
 * @param 　　*dest_array　データの格納先
 *
 */
void getDataFromMPU9250(uint8_t *dest_array)
{
	volatile uint32_t i;
	for(i=0;i<14;i++)
	{
		dest_array[i] = mpu9250ArrayData[i];
	}
}

/**
 * AK8963(MPU9250の地磁気センサ)のデータ(MagXYZ) <br>
 * をdest_arrayに次のように格納 <br>
 * byte    <br>
 * 0    MagX  下位 <br>
 * 1    MagX  上位 <br>
 * 2    MagY  下位 <br>
 * 3    MagY  上位 <br>
 * 4    MagZ  下位 <br>
 * 5    MagZ  上位 <br>
 *
 * @param  *dest_array データの格納先
 *
 */
void getDataFromAK8963(uint8_t *dest_array)
{
	volatile uint32_t i;
	for(i=0;i<6;i++)
	{
		dest_array[i] = ak8963ArrayData[i];
	}
}

/**
 * MPU9250とAK8963(MPU9250の地磁気センサ)のwho am iを読む <br>
 * 1秒ごとにwho am iを呼びだし続ける無限ループが記述されている <br>
 *
 * @param void
 * @return void
 */
void whoAmI(void)
{
	uint16_t i;
	setSendDataEnable(0);
	while(1)
	{
		myPrintfUSB("//////////////// \n");

		I2CWriteLength = 2;
		I2CReadLength = 1;
		I2CMasterBuffer[0] = AK8963_W;
		I2CMasterBuffer[1] = 0x00; //Address 3byte (adjust MagXYZ)
		I2CMasterBuffer[2] = AK8963_R;
		I2CEngine();

		myPrintfUSB("Who Am I AK8963 : %d \n", I2CSlaveBuffer[0]);

		for ( i = 0; i < BUFSIZE; i++ ) //clear I2CSlaveBuffer
		{
			I2CSlaveBuffer[i] = 0x00;
		}

		wait1msec(1000);
		myPrintfUSB("//////////////// \n");

		I2CWriteLength = 2;
		I2CReadLength = 1;
		I2CMasterBuffer[0] = MPU9250_W;
		I2CMasterBuffer[1] = 0x75; //Address 3byte (adjust MagXYZ)
		I2CMasterBuffer[2] = MPU9250_R;
		I2CEngine();

		myPrintfUSB("Who Am I MPU9250 : %d \n", I2CSlaveBuffer[0]);

		for ( i = 0; i < BUFSIZE; i++ ) //clear I2CSlaveBuffer
		{
			I2CSlaveBuffer[i] = 0x00;
		}

		wait1msec(1000);
	}
}

/**
 * 角速度ベクトルを返す.  単位は[rad/s]
 *
 * @param *vec
 */
void getOmega(float *vec)
{
	uint8_t i;
	for(i=0;i<3;i++) vec[i] = omega_vec[i];
};

/**
 * 地磁気ベクトルを返す. 単位は	[uT]
 *
 * @param *vec　格納先
 * @return void
 */
void getMag(float *vec)
{
	uint8_t i;
	for(i=0;i<3;i++) vec[i] = mag_vec[i];
};

/**
 * 加速度ベクトルを返す. 単位は[1G]
 *
 * @param *vec 格納先
 * @return void
 */
void getAcc(float *vec)
{
	uint8_t i;
	for(i=0;i<3;i++) vec[i] = acc_vec[i];
};

/**
 * 温度を返す. 単位は[℃]
 *
 * @param *vec 格納先
 * @return void
 */
void getTemp(float *temp)
{
	*temp = temperature;
}

/**
 * 現在のジャイロセンサのオフセット値を返す
 *
 * @param  *offset_vec
 */
void getOmegaOffset(int16_t *offset_vec)
{
	uint8_t i;
	for(i=0;i<3;i++) offset_vec[i] = omega_offset_vec[i];
}

/**
 * 現在の加速度のオフセット値を返す
 *
 * @param  *offset_vec
 */
void getAccOffset(int16_t *offset_vec)
{
	uint8_t i;
	for(i=0;i<3;i++) offset_vec[i] = acc_offset_vec[i];
}

/**
 * 現在の地磁気のオフセット値を返す
 *
 * @param  *offset_vec
 */
void getMagOffset(int16_t *offset_vec)
{
	uint8_t i;
	for(i=0;i<3;i++) offset_vec[i] = mag_offset_vec[i];
}

/**
 * 角速度のオフセット値をセットする
 *
 * @param  *offset_vec
 */
void setOmegaOffset(int16_t *offset_vec)
{
	uint8_t i;
	for(i=0;i<3;i++) omega_offset_vec[i] = offset_vec[i];
}

/**
 * 加速度のオフセット値をセットする
 *
 * @param  *offset_vec
 */
void setAccOffset(int16_t *offset_vec)
{
	uint8_t i;
	for(i=0;i<3;i++) acc_offset_vec[i] = offset_vec[i];
}

/**
 * 地磁気のオフセット値をセットする
 *
 * @param  *offset_vec
 */
void setMagOffset(int16_t *offset_vec)
{
	uint8_t i;
	for(i=0;i<3;i++) mag_offset_vec[i] = offset_vec[i];
}

/**
 * 角速度のx方向のオフセットをセットする
 *
 * @param  offset_x
 */
void setOmegaOffset_x(int16_t offset_x)
{
	omega_offset_vec[0] = offset_x;
}

/**
 * 角速度のy方向のオフセットをセットする
 *
 * @param  offset_y
 */
void setOmegaOffset_y(int16_t offset_y)
{
	omega_offset_vec[1] = offset_y;
}

/**
 * 角速度のz方向のオフセットをセットする
 *
 * @param  offset_z
 */
void setOmegaOffset_z(int16_t offset_z)
{
	omega_offset_vec[2] = offset_z;
}

/**
 * 加速度のx方向のオフセットをセットする
 *
 * @param  offset_x
 */
void setAccOffset_x(int16_t offset_x)
{
	acc_offset_vec[0] = offset_x;
}

/**
 * 加速度のy方向のオフセットをセットする
 *
 * @param  offset_y
 */
void setAccOffset_y(int16_t offset_y)
{
	acc_offset_vec[1] = offset_y;
}

/**
 * 加速度のz方向のオフセットをセットする
 *
 * @param  offset_x
 */
void setAccOffset_z(int16_t offset_z)
{
	acc_offset_vec[2] = offset_z;
}

/**
 * 地磁気のx方向のオフセットをセットする
 *
 * @param  offset_x
 */
void setMagOffset_x(int16_t offset_x)
{
	mag_offset_vec[0] = offset_x;
}

/**
 * 地磁気のy方向のオフセットをセットする
 *
 * @param  offset_y
 */
void setMagOffset_y(int16_t offset_y)
{
	mag_offset_vec[1] = offset_y;
}

/**
 * 地磁気のz方向のオフセットをセットする
 *
 * @param  offset_z
 */
void setMagOffset_z(int16_t offset_z)
{
	mag_offset_vec[2] = offset_z;
}

/**
 * 地磁気センサのAsa値を返す
 *
 * @param  *vec
 */
void getAsa(int16_t *vec)
{
	uint8_t i;
	for(i=0;i<3;i++) vec[i] = Asa[i];
}

/**
 * 角速度を整数値で返す
 *
 * @param  *vec
 */
void getOmegaInt(int16_t *vec)
{
	uint8_t i;
	for(i=0;i<3;i++) vec[i] = omega_vec_int[i];
}

/**
 *　加速度を整数値で返す
 *
 * @param  *vec
 */
void getACCInt(int16_t *vec)
{
	uint8_t i;
	for(i=0;i<3;i++) vec[i] = acc_vec_int[i];
}

/**
 * 地磁気を整数値で返す
 *
 * @param  *vec
 */
void getMagInt(int16_t *vec)
{
	uint8_t i;
	for(i=0;i<3;i++) vec[i] = mag_vec_int[i];
}

/**
 * 温度を整数値で返す
 *
 * @param  *vec
 */
void getTempInt(int16_t *temp)
{
	*temp = temperature_int;
}

/**
 * 角速度の生データを整数値で返す
 *
 * @param  *vec
 */
void getRawOmegaInt(int16_t *vec)
{
	uint8_t i;
	for(i=0;i<3;i++) vec[i] = omega_vec_raw_int[i];
}

/**
 * 加速度の生データを整数値で返す
 *
 * @param  *vec
 */
void getRawAccInt(int16_t *vec)
{
	uint8_t i;
	for(i=0;i<3;i++) vec[i] = omega_vec_raw_int[i];
}

/**
 * 地磁気の生データを整数値で返す
 *
 * @param  *vec
 */
void getRawMagInt(int16_t *vec)
{
	uint8_t i;
	for(i=0;i<3;i++) vec[i] = mag_vec_raw_int[i];
}

/**
 * 温度の生データを整数値で返す
 *
 * @param  *temp
 */
void getRawTempInt(int16_t *temp)
{
	*temp = temperature_raw_int;
};


/**
 * ジャイロオフセット設定用関数<br>
 * 静止状態でNUM_REF回のサンプルを取り, そのデータの上位25%と下位25%の値を
 * クイックソートによって取り除き残りの50%で平均値を求めオフセットとする.(メディアンフィルタと平均の組み合わせ)
 *
 * @param *offset_vec セットしたoffset値のベクトルを返す
 * @return void
 */
void calibOmegaOffset(int16_t *offset_vec)
{
	uint32_t i = 0;
	#define NUM_REF_OMEGA (200)

	int32_t omega_x[NUM_REF_OMEGA];
	int32_t omega_y[NUM_REF_OMEGA];
	int32_t omega_z[NUM_REF_OMEGA];

	float omega_x_sum = 0.0;
	float omega_y_sum = 0.0;
	float omega_z_sum = 0.0;

	for(i=0;i<3;i++)
	{
		omega_offset_vec[i] = 0;
	}

	for(i=0;i<NUM_REF_OMEGA;i++)
	{
		updateMPU9250Data();
		omega_x[i] = omega_vec_raw_int[0];
		omega_y[i] = omega_vec_raw_int[1];
		omega_z[i] = omega_vec_raw_int[2];
		wait1msec(10);
		//myPrintfUSB("%d| calibrating... %d, %d, %d\n",i,omega_x [i],omega_y[i],omega_z[i] );
	}
	quickSort(omega_x,0 ,NUM_REF_OMEGA -1);
	quickSort(omega_y,0 ,NUM_REF_OMEGA -1);
	quickSort(omega_z,0 ,NUM_REF_OMEGA -1);

	for(i=NUM_REF_OMEGA/4;i<(NUM_REF_OMEGA*3/4 +1) ;i++)
	{
		omega_x_sum += (float)(omega_x[i]);
		omega_y_sum += (float)(omega_y[i]);
		omega_z_sum += (float)(omega_z[i]);
	}
	omega_offset_vec[0] = (int16_t)(omega_x_sum/(float)NUM_REF_OMEGA *2.0);
	omega_offset_vec[1] = (int16_t)(omega_y_sum/(float)NUM_REF_OMEGA *2.0);
	omega_offset_vec[2] = (int16_t)(omega_z_sum/(float)NUM_REF_OMEGA *2.0);

	//writeEEPROMOffsetOmegaInt(&omega_offset_vec[0]);
	//myPrintfUSB("==========\n gyro offset %d, %d, %d\n",omega_offset_vec[0],omega_offset_vec[1],omega_offset_vec[2] );
}

/**
 * 加速度センサのXY方向ロオフセット設定用関数<br>
 * センサを水平にした状態でNUM_REF回のサンプルを取り, そのデータの上位25%と下位25%の値を
 * クイックソートによって取り除く残りの50%で平均値を求めオフセットとする. (メディアンフィルタと平均の組み合わせ)
 *
 * @param void
 * @return void
 */
void calibAccXYOffset()
{
	uint32_t i = 0;
	#define NUM_REF_ACCXY (400)

	int32_t acc_x[NUM_REF_ACCXY];
	int32_t acc_y[NUM_REF_ACCXY];

	float acc_x_sum = 0.0;
	float acc_y_sum = 0.0;

	acc_offset_vec[0] = 0;
	acc_offset_vec[1] = 0;

	for(i=0;i<NUM_REF_ACCXY;i++)
	{
		updateMPU9250Data();
		acc_x[i] = acc_vec_raw_int[0];
		acc_y[i] = acc_vec_raw_int[1];
		wait1msec(10);
		myPrintfUSB("%d| calibrating... %d, %d\n",i,acc_x [i],acc_y[i]);
	}
	quickSort(acc_x,0 ,NUM_REF_ACCXY -1);
	quickSort(acc_y,0 ,NUM_REF_ACCXY -1);

	for(i=NUM_REF_ACCXY/4;i<(NUM_REF_ACCXY*3/4 +1) ;i++)
	{
		acc_x_sum += (float)(acc_x[i]);
		acc_y_sum += (float)(acc_y[i]);
	}
	acc_offset_vec[0] = (int16_t)(acc_x_sum/(float)NUM_REF_ACCXY *2.0);
	acc_offset_vec[1] = (int16_t)(acc_y_sum/(float)NUM_REF_ACCXY *2.0);
	writeEEPROMOffsetAccInt(&acc_offset_vec[0]);
	myPrintfUSB("==========\n acc offsetXY %d, %d\n",acc_offset_vec[0],acc_offset_vec[1]);
}

/**
 * 加速度センサのZ方向ロオフセット設定用関数<br>
 * センサを垂直にした状態でNUM_REF_ACCZ回のサンプルを取り, そのデータの上位25%と下位25%の値を
 * クイックソートによって取り除く残りの50%で平均値を求めオフセットとする. (メディアンフィルタと平均の組み合わせ)
 *
 * @param void
 * @return void
 */
void calibAccZOffset()
{
	uint32_t i = 0;
	#define NUM_REF_ACC (400)

	int32_t acc_z[NUM_REF_ACC];

	float acc_z_sum = 0.0;

	acc_offset_vec[2] = 0;

	for(i=0;i<NUM_REF_ACC;i++)
	{
		updateMPU9250Data();
		acc_z[i] = acc_vec_raw_int[2];
		wait1msec(10);
		myPrintfUSB("%d| calibrating... %d \n",i,acc_z[i]);
	}
	quickSort(acc_z,0 ,NUM_REF_ACC -1);

	for(i=NUM_REF_ACC/4;i<(NUM_REF_ACC*3/4 +1) ;i++)
	{
		acc_z_sum += (float)(acc_z[i]);
	}
	acc_offset_vec[2] = (int16_t)(acc_z_sum/(float)NUM_REF_ACC *2.0);
	writeEEPROMOffsetAccInt(&acc_offset_vec[0]);
	myPrintfUSB("==========\n acc offsetZ %d\n",acc_offset_vec[2]);
}

/**
 * 地磁気センサのオフセット設定用関数<br>
 * センサをしばらくの時間色々な方向に動かした後にすべての各軸の(最大値+最小値)/2を
 * オフセット値とする.  地磁気の影響が少ない環境でオフセット調整をすること.
 *
 * @param void
 * @return void
 */
void calibMagOffset()
{
	uint32_t i = 0;
	#define NUM_REF_MAG (300)

	int32_t mag_x[NUM_REF_MAG];
	int32_t mag_y[NUM_REF_MAG];
	int32_t mag_z[NUM_REF_MAG];

	for(i=0;i<3;i++){
		mag_offset_vec[i] = 0;
	}

	for(i=0;i<NUM_REF_MAG;i++)
	{
		updateAK8963Data();
		mag_x[i] = mag_vec_raw_int[0];
		mag_y[i] = mag_vec_raw_int[1];
		mag_z[i] = mag_vec_raw_int[2];
		wait1msec(100);
		myPrintfUSB("%d| calibrating... %d,%d,%d \n",i ,mag_x[i], mag_y[i], mag_z[i]);
	}

	quickSort(mag_x,0 ,NUM_REF_MAG -1);
	quickSort(mag_y,0 ,NUM_REF_MAG -1);
	quickSort(mag_z,0 ,NUM_REF_MAG -1);

	mag_offset_vec[0] = (int16_t)((mag_x[0] + mag_x[NUM_REF_MAG-1])/2);
	mag_offset_vec[1] = (int16_t)((mag_y[0] + mag_y[NUM_REF_MAG-1])/2);
	mag_offset_vec[2] = (int16_t)((mag_z[0] + mag_z[NUM_REF_MAG-1])/2);
	myPrintfUSB(" mag X min-max  %d,%d\n", mag_x[0],mag_x[NUM_REF_MAG-1]);
	myPrintfUSB(" mag Y min-max  %d,%d\n", mag_y[0],mag_y[NUM_REF_MAG-1]);
	myPrintfUSB(" mag Z min-max  %d,%d\n", mag_z[0],mag_z[NUM_REF_MAG-1]);

	writeEEPROMOffsetMagInt(&mag_offset_vec[0]);
	myPrintfUSB("==========\n mag offset %d,%d,%d\n", mag_offset_vec[0],mag_offset_vec[1],mag_offset_vec[2]);
}

/**
 * デバッグ用関数
 *
 * @param  void
 * @return void
 */
void debugMPU9250(void)
{
	myPrintfUSB("########debug MPU9250#######\n");
	wait1msec(5);
	myPrintfUSB("------float data------\n");
	myPrintfUSB("acc  : %f, %f, %f  (x,y,z)[1G] \n", acc_vec[0], acc_vec[1],acc_vec[2]);
	myPrintfUSB("omega: %f, %f, %f  (x,y,z) [rad] \n", omega_vec[0], omega_vec[1],omega_vec[2]);
	myPrintfUSB("mag  : %f, %f, %f  (x,y,z) [uT] \n", mag_vec[0], mag_vec[1],mag_vec[2]);
	myPrintfUSB("temp : %f                  [°C] \n", temperature);
	wait1msec(5);
	myPrintfUSB("------int data------\n");
	myPrintfUSB("acc  : %d, %d, %d  (x,y,z) \n", acc_vec_int[0], acc_vec_int[1],acc_vec_int[2]);
	myPrintfUSB("omega: %d, %d, %d  (x,y,z) \n", omega_vec_int[0], omega_vec_int[1],omega_vec_int[2]);
	myPrintfUSB("mag  : %d, %d, %d  (x,y,z) \n", mag_vec_int[0], mag_vec_int[1],mag_vec_int[2]);
	myPrintfUSB("temp : %d                  \n", temperature_int);
	wait1msec(5);
	myPrintfUSB("------raw int data------\n");
	myPrintfUSB("acc  : %d, %d, %d  (x,y,z) \n", acc_vec_raw_int[0], acc_vec_raw_int[1],acc_vec_raw_int[2]);
	myPrintfUSB("omega: %d, %d, %d  (x,y,z) \n", omega_vec_raw_int[0], omega_vec_raw_int[1],omega_vec_raw_int[2]);
	myPrintfUSB("mag  : %d, %d, %d  (x,y,z) \n", mag_vec_raw_int[0], mag_vec_raw_int[1],mag_vec_raw_int[2]);
	myPrintfUSB("temp : %d                  \n", temperature_raw_int);
	wait1msec(5);
	myPrintfUSB("------offset data------\n");
	myPrintfUSB("acc  : %d, %d, %d  (x,y,z) \n", acc_offset_vec[0], acc_offset_vec[1],acc_offset_vec[2]);
	myPrintfUSB("omega: %d, %d, %d  (x,y,z) \n", omega_offset_vec[0], omega_offset_vec[1],omega_offset_vec[2]);
	myPrintfUSB("mag  : %d, %d, %d  (x,y,z) \n", mag_offset_vec[0], mag_offset_vec[1],mag_offset_vec[2]);
	wait1msec(5);
	myPrintfUSB("------ASA data------\n");
	myPrintfUSB("Asa  : %d, %d, %d (x,y,z)\n",Asa[1],Asa[0],Asa[2]); //地磁気センサの出力」は軸を入れ替えているので注意
	wait1msec(5);
}

/**
 * 加速度のオフセット値をEEPROMへ書き込む<br>
 * 加速度のオフセット値はEEPROMの0ブロック目,0x0番地から6byte分
 * にX,Y,Zの順で書き込まれる
 *
 * @param  *vec 加速度のオフセット値が格納されている配列
 */
void writeEEPROMOffsetAccInt(int16_t *vec)
{
	writeNbyteEEPROM(0,0,(uint8_t *)vec ,6);
};

/**
 * 角速度のオフセット値をEEPROMへ書き込む<br>
 * 角速度のオフセット値はEEPROMの0ブロック目,0x6番地から6byte分
 * にX,Y,Zの順で書き込まれる
 *
 * @param  *vec 角速度のオフセット値が格納されている配列
 */
void writeEEPROMOffsetOmegaInt(int16_t *vec)
{
	writeNbyteEEPROM(0,6,(uint8_t *)vec,6);
};

/**
 * 地磁気のオフセット値をEEPROMへ書き込む<br>
 * 地磁気のオフセット値はEEPROMの0ブロック目,0x0C番地から6byte分
 * にX,Y,Zの順で書き込まれる
 *
 * @param  *vec 地磁気のオフセット値が格納されている配列
 */
void writeEEPROMOffsetMagInt(int16_t *vec)
{
	writeNbyteEEPROM(0,12,(uint8_t *)vec,6);
};

/**
 * EEPROMから読み出した加速度のオフセット値を配列に読み込む<br>
 * @param  *vec 加速度のオフセット値を格納する先の配列
 */
void readEEPROMOffsetAccInt(int16_t *vec)
{
	readNbyteEEPROM(0,0,(uint8_t *)vec,6);
};

/**
 * EEPROMから読み出した角速度のオフセット値を配列に読み込む<br>
 * @param  *vec 角速度のオフセット値を格納する先の配列
 */
void readEEPROMOffsetOmegaInt(int16_t *vec)
{
	readNbyteEEPROM(0,6,(uint8_t *)vec,6);
};

/**
 * EEPROMから読み出した地磁気のオフセット値を配列に読み込む<br>
 * @param  *vec 地磁気のオフセット値を格納する先の配列
 */
void readEEPROMOffsetMagInt(int16_t *vec)
{
	readNbyteEEPROM(0,12,(uint8_t *)vec,6);
}

/**
 * EEPROMから角速度, 加速度, 地磁気のオフセット値を読み取り
 * オフセット格納用の配列にセットする.
 *
 * @param  void
 * @return void
 */
void initOffsetByEEPROM()
{
	readEEPROMOffsetAccInt(acc_offset_vec);
	readEEPROMOffsetOmegaInt(omega_offset_vec);
	readEEPROMOffsetMagInt(mag_offset_vec);
}

/**
 * 16bit整数の上位byteと下位byteを結合する
 *
 * @param  H_byte 上位byte
 * @param  L_byte 下位byte
 * @return void
 */
int16_t concatenate2Byte_int(uint8_t H_byte, uint8_t L_byte)
{
    int32_t con;
    con = L_byte + (H_byte<<8);
    if (con > 32767)
    {
      con -=  65536;
    }
    return (int16_t)con;
}

/**
 * 加速度と角速度の値からセンサが静止しているかを判定する
 *
 * @return 1: 静止している    0:動いている
 */
uint8_t isStop(){
	uint8_t flag_stop = 0;
	float acc_norm = tSqrt(acc_vec[0]*acc_vec[0]+acc_vec[1]*acc_vec[1]+acc_vec[2]*acc_vec[2]);
	uint8_t i = 0;

	if( ABS(1.0 - acc_norm )<0.12 )
	{
		for(i=0;i<3;i++)
		{
			if(ABS(omega_vec[0]) < DEG2RAD(2) &&
			   ABS(omega_vec[1]) < DEG2RAD(2) &&
			   ABS(omega_vec[2]) < DEG2RAD(2)
			) flag_stop = 1;
		}
	}
	return flag_stop;
}
