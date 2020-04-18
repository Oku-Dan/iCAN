/*

	2019 Oku Dan / oku_dan@yahoo.co.jp

	MPU6050用のI2C版ライブラリです.
	使用する際はデバイスのSDAとSCLをセンサに接続してください.

	1.MPU6050型クラスを作成
	2.Initialize()でセンサ初期化
	3.ReadAccGyr()で読み出し

	内部DMPを利用してかなり正確なQuaternionを出力してくれる裏ワザ的手法がありますが、かなりの魔境なので追加しません.
	利用したい方は自分で調べてください.
*/

#include "I2CHandler.h"

#define MPU6050_ADDRESS 0x68

#define MPU6050_WHO_AM_I 0x75
#define MPU6050_WHO_AM_I_DEFAULT 0x68
#define MPU6050_PWR_MGMT_1 0x6B
#define MPU6050_INT_PIN_CFG 0x37

#define MPU6050_ACCEL_CONFIG 0x1C
#define MPU6050_ACCEL_FS_SEL_2G 0x00
#define MPU6050_ACCEL_FS_SEL_4G 0x08
#define MPU6050_ACCEL_FS_SEL_8G 0x10
#define MPU6050_ACCEL_FS_SEL_16G 0x18

#define MPU6050_GYRO_CONFIG 0x1B
#define MPU6050_GYRO_FS_SEL_250DPS 0x00
#define MPU6050_GYRO_FS_SEL_500DPS 0x08
#define MPU6050_GYRO_FS_SEL_1000DPS 0x10
#define MPU6050_GYRO_FS_SEL_2000DPS 0x18

#define MPU6050_ACCEL_XOUT_H 0x3B
#define MPU6050_ACCEL_XOUT_L 0x3C
#define MPU6050_ACCEL_YOUT_H 0x3D
#define MPU6050_ACCEL_YOUT_L 0x3E
#define MPU6050_ACCEL_ZOUT_H 0x3F
#define MPU6050_ACCEL_ZOUT_L 0x40
#define MPU6050_TEMP_OUT_H 0x41
#define MPU6050_TEMP_OUT_L 0x42
#define MPU6050_GYRO_XOUT_H 0x43
#define MPU6050_GYRO_XOUT_L 0x44
#define MPU6050_GYRO_YOUT_H 0x45
#define MPU6050_GYRO_YOUT_L 0x46
#define MPU6050_GYRO_ZOUT_H 0x47
#define MPU6050_GYRO_ZOUT_L 0x48


class MPU6050
{
private:
	float accRange;
	float gyroRange;

public:
	bool Initialize();
	bool Initialize(int AccelScale, int GyroScale);
	void ReadAccGyr(float *ax, float *ay, float *az, float *gx, float *gy, float *gz);
	void ReadAcc(float *ax, float *ay, float *az);
	void ReadGyr(float *gx, float *gy, float *gz);
};


/*
#include <Wire.h>

void I2cWriteByte(uint8_t add, uint8_t reg, uint8_t data)
{
	Wire.beginTransmission(add);
	Wire.write(reg);
	Wire.write(data);
	Wire.endTransmission();
}

uint8_t I2cReadByte(uint8_t add, uint8_t reg)
{
	Wire.beginTransmission(add);
	Wire.write(reg);
	Wire.endTransmission(false);
	Wire.requestFrom(add, (uint8_t)1);
	uint8_t data = Wire.read();
	return data;
}

void I2cReadBytes(uint8_t add, uint8_t reg, uint8_t *data, uint8_t count)
{
	Wire.beginTransmission(add);
	Wire.write(reg);
	Wire.endTransmission(false);
	Wire.requestFrom(add, count);
	for (int i = 0; i < count; i++)
	{
		data[i] = Wire.read();
	}
}

void I2cInitialize(){
	Wire.begin();
	Wire.setClock( 400000L );
}
*/

bool MPU6050::Initialize(int AccelScale, int GyroScale){
	uint8_t tempRegVal;
	I2cInitialize();
	tempRegVal = I2cReadByte(MPU6050_ADDRESS, MPU6050_WHO_AM_I);
	if(tempRegVal != MPU6050_WHO_AM_I_DEFAULT)return false;

	I2cWriteByte(MPU6050_ADDRESS, MPU6050_PWR_MGMT_1, 0);				 //スリープモードを解除
	switch (AccelScale)
	{
	case 2:
		tempRegVal = MPU6050_ACCEL_FS_SEL_2G;
		accRange = 2.0;												 //計算で使用するので，選択したレンジを入力する
		break;
	case 4:
		tempRegVal = MPU6050_ACCEL_FS_SEL_4G;
		accRange = 4.0;
		break;
	case 8:
		tempRegVal = MPU6050_ACCEL_FS_SEL_8G;
		accRange = 8.0;
		break;	
	default:
		tempRegVal = MPU6050_ACCEL_FS_SEL_16G;
		accRange = 16.0;
		break;
	}
	I2cWriteByte(MPU6050_ADDRESS, MPU6050_ACCEL_CONFIG, tempRegVal);		 //加速度センサの測定レンジの設定

	switch (GyroScale)
	{
	case 250:
		tempRegVal = MPU6050_GYRO_FS_SEL_250DPS;
		gyroRange = 250.0;												 //計算で使用するので，選択したレンジを入力する
		break;
	case 500:
		tempRegVal = MPU6050_GYRO_FS_SEL_500DPS;
		gyroRange = 500.0;
		break;
	case 1000:
		tempRegVal = MPU6050_GYRO_FS_SEL_1000DPS;
		gyroRange = 1000.0;
		break;
	default:
		tempRegVal = MPU6050_GYRO_FS_SEL_2000DPS;
		gyroRange = 2000.0;
		break;
	}
	I2cWriteByte(MPU6050_ADDRESS, MPU6050_GYRO_CONFIG, tempRegVal); //ジャイロセンサの測定レンジの設定

	accRange /= 32768.0;	//計算用の係数に変換
	gyroRange /= 32768.0;

	return true;
}

bool::MPU6050::Initialize()
{
	return Initialize(16, 2000);
}

void MPU6050::ReadAccGyr(float *ax, float *ay, float *az, float *gx, float *gy, float *gz){
	uint8_t AccGyroTemp[14];
	I2cReadBytes(MPU6050_ADDRESS, MPU6050_ACCEL_XOUT_H, AccGyroTemp, 14);
	*ax = (int16_t)(AccGyroTemp[0] << 8 | AccGyroTemp[1]) * accRange;
	*ay = (int16_t)(AccGyroTemp[2] << 8 | AccGyroTemp[3]) * accRange;
	*az = (int16_t)(AccGyroTemp[4] << 8 | AccGyroTemp[5]) * accRange;

	*gx = (int16_t)(AccGyroTemp[8] << 8 | AccGyroTemp[9]) * gyroRange;
	*gy = (int16_t)(AccGyroTemp[10] << 8 | AccGyroTemp[11]) * gyroRange;
	*gz = (int16_t)(AccGyroTemp[12] << 8 | AccGyroTemp[13]) * gyroRange;
}

void MPU6050::ReadAcc(float *ax, float *ay, float *az){
	uint8_t AccTemp[6];
	I2cReadBytes(MPU6050_ADDRESS, MPU6050_ACCEL_XOUT_H, AccTemp, 6);
	*ax = (int16_t)(AccTemp[0] << 8 | AccTemp[1]) * accRange;
	*ay = (int16_t)(AccTemp[2] << 8 | AccTemp[3]) * accRange;
	*az = (int16_t)(AccTemp[4] << 8 | AccTemp[5]) * accRange;
}
void MPU6050::ReadGyr(float *gx, float *gy, float *gz){
	
	uint8_t GyroTemp[6];
	I2cReadBytes(MPU6050_ADDRESS, MPU6050_GYRO_XOUT_H, GyroTemp, 6);
	*gx = (int16_t)(GyroTemp[0] << 8 | GyroTemp[1]) * gyroRange;
	*gy = (int16_t)(GyroTemp[2] << 8 | GyroTemp[3]) * gyroRange;
	*gz = (int16_t)(GyroTemp[4] << 8 | GyroTemp[5]) * gyroRange;
}