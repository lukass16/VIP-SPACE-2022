#include <Arduino.h>
#include "MPU9250.h"

MPU9250 mpu; // You can also use MPU9255 as is

void setup()
{
	Serial.begin(115200);
	Serial.println("Testing alternate library with MPU9250.");
	Wire.begin(21, 22);
	delay(2000);
	mpu.setup(0x68);

	//*test calibration
	mpu.calibrateAccelGyro();
}

void loop()
{
	if (mpu.update())
	{
		//*Get readings from accelerometer + gyro
		Serial.print(mpu.getAccX());
		Serial.print(", ");
		Serial.print(mpu.getAccY());
		Serial.print(", ");
		Serial.print(mpu.getAccZ());

		Serial.print("\t");
		Serial.print(mpu.getGyroX());
		Serial.print(", ");
		Serial.print(mpu.getGyroY());
		Serial.print(", ");
		Serial.println(mpu.getGyroZ());
	}
	delay(1000);
}