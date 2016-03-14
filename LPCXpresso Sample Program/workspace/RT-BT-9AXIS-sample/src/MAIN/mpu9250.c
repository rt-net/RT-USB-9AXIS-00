/**
 * @file   mpu9250.c
 * @brief  MPU9250を使えるようにするための関数群 <br>
 *
 * @author RTCorp. Ryota Takahashi
 */

#include "type.h"
#include "mpu9250.h"
#include "i2c.h"
#include "uart.h"
#include "SystemTickTimer.h"
#include "MainFunction.h"
#include "TinyMathFunction.h"
#include "debug.h"


//i2cの操作に必要なもの達のextern宣言.  実体はi2c.cにある
extern volatile uint8_t  I2CMasterBuffer[BUFSIZE];
extern volatile uint8_t  I2CSlaveBuffer[BUFSIZE];
extern volatile uint32_t I2CReadLength, I2CWriteLength;

//MPU9250のi2cのアドレス
#define MPU9250_W		0xd0
#define MPU9250_R		0xd1
#define AK8963_W		0x18
#define AK8963_R		0x19

static volatile uint8_t Asa[]             = {128, 128, 128};         //AK8963の感度補正データ格納用(感度補正データは工場出荷時にICに書き込まれている)

static volatile float omega_vec[]         = {0.0, 0.0, 0.0};   //角速度５ベクトル (x,y,z)[rad]
static volatile float acc_vec[]           = {0.0, 0.0, 0.0};   //加速度ベクトル (x,y,z) [1G]
static volatile float mag_vec[]           = {0.0, 0.0, 0.0};   //地磁気ベクトル (x,y,z) [uT]
static volatile float temperature         = 0.0;               //MPU9250の温度[°C]

static volatile float omega_LPF_vec[] = {0.0, 0.0, 0.0};       //軽めのローパスフィルタをかけた値
static volatile float acc_LPF_vec[]   = {0.0, 0.0, 0.0};
static volatile float mag_LPF_vec[]   = {0.0, 0.0, 0.0};

static volatile float omega_str_LPF_vec[] = {0.0, 0.0, 0.0};   //強めのローパスフィルタをかけた値
static volatile float acc_str_LPF_vec[]   = {0.0, 0.0, 0.0};
static volatile float mag_str_LPF_vec[]   = {0.0, 0.0, 0.0};

static volatile float omega_ref_vec[]     = {0.0,0.0,0.0};     //静止時のジャイロリファレンス

volatile float gain_LPF     = 0.8;      //ローパスフィルタのゲイン
volatile float gain_str_LPF = 0.1;     //強めのローパスフィルタのゲイン

/**
 * MPU9250の初期化(MPU9250のジャイロ,加速度センサ)
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
	I2CMasterBuffer[1] = 0x37; //Address auxiliary I2C
	I2CMasterBuffer[2] = 0x02;
	I2CEngine();

	wait1msec(10);

	I2CWriteLength = 3;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = MPU9250_W;
	I2CMasterBuffer[1] = 0x1A; //Address digital low pass filter
	I2CMasterBuffer[2] = 0x02; //100Hz
	I2CEngine();

	wait1msec(10);

	I2CWriteLength = 3;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = MPU9250_W;
	I2CMasterBuffer[1] = 0x1B; //range ang
	I2CMasterBuffer[2] = 0x18; //2000deg/s
	I2CEngine();

	wait1msec(10);

	I2CWriteLength = 3;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = MPU9250_W;
	I2CMasterBuffer[1] = 0x1C; //range acc
	I2CMasterBuffer[2] = 0x18; //16g
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
	I2CMasterBuffer[2] = 0x10; //16bitの分解能に変更
	I2CEngine();

	wait1msec(10);


}

/**
 * MPU9250のデータ (AccXYZ + GyroXYZ + temperature) <br>
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
 * さらに, センサの読み取りデータを物理量に変換し, acc_vec[i] <br>
 * omega_vec[i]に格納する.
 *
 * @param 　　*dest_array　データの格納先
 * @return  void
 */
void getDataFromMPU9250 (uint8_t *dest_array )
{
	volatile uint32_t i;
	volatile int32_t acc[3];
	volatile uint16_t acc_temp[3]; //加速度の値計算の中間値
	volatile int32_t omega[3];
	volatile int32_t temp_;  //温度

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
	dest_array[0]  =  0x00ff & acc_temp[0];
	dest_array[1]  = (0xff00 & acc_temp[0])>>8;
	dest_array[2]  =  0x00ff & acc_temp[1];
	dest_array[3]  = (0xff00 & acc_temp[1])>>8;
	dest_array[4]  =  0x00ff & acc_temp[2];
	dest_array[5]  = (0xff00 & acc_temp[2])>>8;
	//temparature
	dest_array[6]  = I2CSlaveBuffer[7];
	dest_array[7]  = I2CSlaveBuffer[6];
	//gyro
	dest_array[8]  = I2CSlaveBuffer[9];
	dest_array[9]  = I2CSlaveBuffer[8];
	dest_array[10] = I2CSlaveBuffer[11];
	dest_array[11] = I2CSlaveBuffer[10];
	dest_array[12] = I2CSlaveBuffer[13];
	dest_array[13] = I2CSlaveBuffer[12];

	for ( i = 0; i < BUFSIZE; i++ ) //clear I2CSlaveBuffer
	{
		I2CSlaveBuffer[i] = 0x00;
	}

	//センサ値を物理量に変換

	acc[0]   = dest_array[0] + (dest_array[1]<<8);
	if(acc[0] >= 32767) acc[0] -= 65536;
	acc_vec[0]  = ((float)acc[0])/2048.0;

	acc[1]   = dest_array[2] + (dest_array[3]<<8);
	if(acc[1] >= 32767) acc[1] -= 65536;
	acc_vec[1]  = ((float)acc[1])/2048.0;

	acc[2]   = dest_array[4] + (dest_array[5]<<8);
	if(acc[2] >= 32767) acc[2] -= 65536;
	acc_vec[2]  = ((float)acc[2])/2048.0;

	omega[0]   = dest_array[8] + (dest_array[9]<<8);
	if(omega[0] >= 32767) omega[0] -= 65536;
	omega_vec[0]  = DEG2RAD( ((omega[0]) - (int32_t)(omega_ref_vec[0] + 0.5 )  )/16.4 );

	omega[1]   = dest_array[10] + (dest_array[11]<<8);
	if(omega[1] >= 32767) omega[1] -= 65536;
	omega_vec[1]  = DEG2RAD( ((omega[1]) - (int32_t)(omega_ref_vec[1] + 0.5 ) )/16.4 );

	omega[2]   = dest_array[12] + (dest_array[13]<<8);
	if(omega[2] >= 32767) omega[2] -= 65536;
	omega_vec[2]  = DEG2RAD( ((omega[2]) - (int32_t)(omega_ref_vec[2] + 0.5 ) )/16.4 );

	temp_  = dest_array[6] + (dest_array[7]<<8);
	if(temp_ >= 32767) temp_ -= 65536;
	temperature = (float)temp_/333.87 + 21.0;

    //加速度センサ値をLPFにかける
	acc_LPF_vec[0]     = gain_LPF * acc_LPF_vec[0] + (1.0-gain_LPF) * acc_vec[0];
	acc_LPF_vec[1]     = gain_LPF * acc_LPF_vec[1] + (1.0-gain_LPF) * acc_vec[1];
	acc_LPF_vec[2]     = gain_LPF * acc_LPF_vec[2] + (1.0-gain_LPF) * acc_vec[2];

	//角速度をLPFにかける
	omega_LPF_vec[0]    = gain_LPF * omega_LPF_vec[0] + (1.0-gain_LPF) * omega_vec[0];
	omega_LPF_vec[1]    = gain_LPF * omega_LPF_vec[1] + (1.0-gain_LPF) * omega_vec[1];
	omega_LPF_vec[2]    = gain_LPF * omega_LPF_vec[2] + (1.0-gain_LPF) * omega_vec[2];

	//加速度センサ値をLPFにかける
	acc_str_LPF_vec[0]     = gain_str_LPF * acc_LPF_vec[0] + (1.0-gain_str_LPF) * acc_vec[0];
	acc_str_LPF_vec[1]     = gain_str_LPF * acc_LPF_vec[1] + (1.0-gain_str_LPF) * acc_vec[1];
	acc_str_LPF_vec[2]     = gain_str_LPF * acc_LPF_vec[2] + (1.0-gain_str_LPF) * acc_vec[2];

	//角速度をLPFにかける
	omega_str_LPF_vec[0]   = gain_str_LPF * omega_str_LPF_vec[0] + (1.0-gain_str_LPF) * omega_vec[0];
	omega_str_LPF_vec[1]   = gain_str_LPF * omega_str_LPF_vec[1] + (1.0-gain_str_LPF) * omega_vec[1];
	omega_str_LPF_vec[2]   = gain_str_LPF * omega_str_LPF_vec[2] + (1.0-gain_str_LPF) * omega_vec[2];
}

/**
 * AK8963(MPU9250の地磁気センサ)のデータ(MagXYZ) <br>
 * をdest_arrayに次のように格納 <br>
 * Asa[]: 工場出荷時にAK8963に書きこまれた補正値 <br>
 * AK8963の初期化の際にAsa[]に格納 <br>
 * byte    <br>
 * 0    MagX  下位 <br>
 * 1    MagX  上位 <br>
 * 2    MagY  下位 <br>
 * 3    MagY  上位 <br>
 * 4    MagZ  下位 <br>
 * 5    MagZ  上位 <br>
 * さらにセンサの読み取り値を物理量に変換しmag_vec[i]に格納する.
 *
 * @param  *dest_array データの格納先
 * @return void
 */
void getDataFromAK8963 (uint8_t *dest_array)
{
	volatile int32_t mag[3];

	volatile uint32_t i;
	volatile int16_t Mag[3];

	I2CWriteLength = 2;
	I2CReadLength = 6;
	I2CMasterBuffer[0] = AK8963_W;
	I2CMasterBuffer[1] = 0x03; //Address 6byte (MagXYZ)
	I2CMasterBuffer[2] = AK8963_R;
	I2CEngine();

	for ( i = 0; i < 3; i++ )
	{
		Mag[i] = (uint16_t)I2CSlaveBuffer[i<<1] | (((uint16_t)I2CSlaveBuffer[(i<<1)+1])<<8);
		Mag[i] = Mag[i]*(uint16_t)((((uint8_t)Asa[i] - 128)/256) + 1);
	}

	dest_array[0] = (uint8_t)Mag[1];
	dest_array[1] = (uint8_t)(Mag[1]>>8);

	dest_array[2] = (uint8_t)Mag[0];
	dest_array[3] = (uint8_t)(Mag[0]>>8);

	Mag[2] = (~Mag[2]) + 1; //地磁気センサのZ軸の取り方を下向きから上向きに変更
	dest_array[4] = (uint8_t)Mag[2];
	dest_array[5] = (uint8_t)(Mag[2]>>8);

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
		mag[i]   = dest_array[i] + (dest_array[i+1]<<8);
		if(mag[i] >= 32767) mag[i] -= 65535;
		mag_vec[i]  = (float)mag[i]*0.15;
	}
}


/**
 * AK8963(MPU9250の地磁気センサ)のwho am iを読む <br>
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
 * 角速度ベクトルを返す
 *
 * @param *vec
 * @return void
 */
void getOmega(float *vec)
{
	uint8_t i;
	for(i=0;i<3;i++) vec[i] = omega_vec[i];
};

/**
 * 地磁気ベクトルを返す
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
 * 加速度ベクトルを返す
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
 * 温度を返す
 *
 * @param *vec 格納先
 * @return void
 */
void getTemp(float *temp)
{
	*temp = temperature;
}

/**
 * ジャイロの静止時のリファレンス取得用関数
 *
 * @param void
 * @return void
 */
void initOmegaRef()
{
	uint32_t i = 0;
	uint8_t data_array[14];
	#define NUM_REF (100)

	int32_t omega_x[NUM_REF];
	int32_t omega_y[NUM_REF];
	int32_t omega_z[NUM_REF];

	float omega_x_sum = 0.0;
	float omega_y_sum = 0.0;
	float omega_z_sum = 0.0;

	for(i=0;i<3;i++)
	{
		omega_ref_vec[i] = 0.0;
	}

	for(i=0;i<NUM_REF;i++)
	{
		getDataFromMPU9250(data_array);
		omega_x[i]   = data_array[8] + (data_array[9]<<8);
		if(omega_x[i] >= 32767) omega_x[i] -= 65536;

		omega_y[i]   = data_array[10] + (data_array[11]<<8);
		if(omega_y[i] >= 32767) omega_y[i] -= 65536;

		omega_z[i]   = data_array[12] + (data_array[13]<<8);
		if(omega_z[i] >= 32767) omega_z[i] -= 65536;
		wait1usec(10000);
	}
	quickSort(omega_x,0 ,NUM_REF -1);
	quickSort(omega_y,0 ,NUM_REF -1);
	quickSort(omega_z,0 ,NUM_REF -1);

	for(i=NUM_REF/4;i<(NUM_REF*3/4 +1) ;i++)
	{
		omega_x_sum += (float)(omega_x[i]);
		omega_y_sum += (float)(omega_y[i]);
		omega_z_sum += (float)(omega_z[i]);
	}
	omega_ref_vec[0] = omega_x_sum/(float)NUM_REF *2.0;
	omega_ref_vec[1] = omega_y_sum/(float)NUM_REF *2.0;
	omega_ref_vec[2] = omega_z_sum/(float)NUM_REF *2.0;

}

/**
 * ジャイロセンサ値にローパスフィルタをかけた値を*vecに格納
 *
 * @param  *vec
 * @return void
 */
void getOmegaLPF(float *vec)
{
	uint8_t i;
	for(i=0;i<3;i++) vec[i] = omega_LPF_vec[i];
};

/**
 * 地磁気センサ値にローパスフィルタをかけた値を*vecに格納
 *
 * @param  *vec
 * @return void
 */
void getMagLPF(float *vec)
{
	uint8_t i;
	for(i=0;i<3;i++) vec[i] = mag_LPF_vec[i];
};


/**
 * 加速度センサ値にローパスフィルタをかけた値を*vecに格納
 *
 * @param  *vec
 * @return void
 */
void getAccLPF(float *vec)
{
	uint8_t i;
	for(i=0;i<3;i++) vec[i] = acc_LPF_vec[i];
};

/**
 * ジャイロセンサ値に強めのローパスフィルタをかけた値を*vecに格納
 *
 * @param  *vec
 * @return void
 */
void getOmegaStrLPF(float *vec)
{
	uint8_t i;
	for(i=0;i<3;i++) vec[i] = omega_LPF_vec[i];
};

/**
 * 地磁気センサ値に強めのローパスフィルタをかけた値を*vecに格納
 *
 * @param  *vec
 * @return void
 */
void getMagStrLPF(float *vec)
{
	uint8_t i;
	for(i=0;i<3;i++) vec[i] = mag_LPF_vec[i];
};

/**
 * 加速度センサ値に強めのローパスフィルタをかけた値を*vecに格納
 *
 * @param  *vec
 * @return void
 */
void getAccStrLPF(float *vec)
{
	uint8_t i;
	for(i=0;i<3;i++) vec[i] = acc_LPF_vec[i];
};

/**
 * 現在のジャイロセンサの静止時のリファレンス値を返す
 *
 * @param  *vec
 * @return *vec
 */
float getOmegaRef(float *vec)
{
	uint8_t i;
	for(i=0;i<3;i++) vec[i] = omega_ref_vec[i];
}

/**
 * ジャイロのリファレンス値ref_xを書き換える.
 *
 * @param  ref_x
 * @return void
 */
void setOmegaRef_x(float ref_x)
{
	omega_ref_vec[0] = ref_x;
}

/**
 * ジャイロのリファレンス値ref_yを書き換える.
 *
 * @param  ref_y
 * @return void
 */
void setOmegaRef_y(float ref_y)
{
	omega_ref_vec[1] = ref_y;
}

/**
 * ジャイロのリファレンス値ref_zを書き換える.
 *
 * @param  ref_z
 * @return void
 */
void setOmegaRef_z(float ref_z)
{
	omega_ref_vec[2] = ref_z;
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
	myPrintfUSB("acc  : %f, %f, %f  (x,y,z)[rad] \n", acc_vec[0], acc_vec[1],acc_vec[2]);
	myPrintfUSB("omega: %f, %f, %f  (x,y,z) [1G] \n", omega_vec[0], omega_vec[1],omega_vec[2]);
	myPrintfUSB("mag  : %f, %f, %f  (x,y,z) [uT] \n", mag_vec[0], mag_vec[1],mag_vec[2]);
	myPrintfUSB("temp : %f                  [°C] \n", temperature);
}


