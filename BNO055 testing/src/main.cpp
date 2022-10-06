#include "imu_wrapper_BNO055.h"

/* Set the delay between fresh samples */
uint16_t BNO055_SAMPLERATE_DELAY_MS = 8;

void setup(void)
{
	Serial.begin(115200);
	imu::setup();
}

void loop(void)
{
	imu::readSensor();
	imu::printAll();

	delay(BNO055_SAMPLERATE_DELAY_MS);
}



// Serial.print("Mode: ");
// Serial.println(bno.getMode());
// uint8_t status, test_res, err;
// bno.getSystemStatus(&status, &test_res, &err);
// Serial.print("Status: ");
// Serial.println(status);