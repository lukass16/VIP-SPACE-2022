#include <Arduino.h>
#include "buzzer.h"
#include "lora_wrapper.h"
#include "barometer_wrapper_MS5607.h"
#include "imu_wrapper_MPU9250.h"
#include "SD_card.h"
#include "gps_wrapper.h"

#define EEPROMCLEAR 37
#define SW1 38
#define SW2 39
#define PYROSENSE 36
#define FIREPYRO 32

unsigned long start_time = millis();
int interval = 10000; // milliseconds

void setup()
{
	Serial.begin(115200);

	// 1) Buzzer
	Serial.println("\nTesting Buzzer");
	buzzer::setup();
	buzzer::signalTestStart();
	buzzer::transitionToGeneratorMode();

	buzzer::signalTransition();
	// 2) LoRa
	Serial.println("\nTesting LoRa");
	lora::setup();
	String message = "TESTING LORA";
	start_time = millis();
	while (millis() - start_time < interval)
	{
		lora::sendMessage(message, 1);
		delay(400);
	}

	buzzer::signalTransition();
	// 3) Barometer
	Wire.begin(21, 22); // initialize correct i2c lines
	Serial.println("\nTesting Barometer");
	barometer::setup();
	start_time = millis();
	while (millis() - start_time < interval)
	{
		barometer::readSensor();
		barometer::printAll();
		delay(400);
	}

	buzzer::signalTransition();
	// 4) IMU
	Serial.println("\nTesting IMU");
	imu::setup();
	start_time = millis();
	while (millis() - start_time < interval)
	{
		imu::readSensor();
		imu::printAll();
		delay(400);
	}

	buzzer::signalTransition();
	// 5) SD
	Serial.println("\nTesting SD");
	SDcard::setup();
	SD_File file = SDcard::openFile();
	start_time = millis();
	while (millis() - start_time < interval)
	{
		SDcard::test(file);
		delay(400);
	}
	SDcard::closeFile(file);

	buzzer::signalTransition();
	// 6) EEPROM clear pin
	Serial.println("\nTesting EEPROM clear pin");
	pinMode(EEPROMCLEAR, INPUT);
	start_time = millis();
	while (millis() - start_time < interval)
	{
		Serial.println("EEPROM clear Pin reads: " + String(analogRead(EEPROMCLEAR)));
		delay(400);
	}

	buzzer::signalTransition();
	// 7) Switch 1
	Serial.println("\nTesting Switch 1");
	pinMode(SW1, INPUT);
	start_time = millis();
	while (millis() - start_time < interval)
	{
		Serial.println("Switch 1 reads: " + String(analogRead(SW1)));
		delay(400);
	}

	buzzer::signalTransition();
	// 8) Switch 2
	Serial.println("\nTesting Switch 2");
	pinMode(SW2, INPUT);
	start_time = millis();
	while (millis() - start_time < interval)
	{
		Serial.println("Switch 2 reads: " + String(analogRead(SW2)));
		delay(400);
	}

	buzzer::signalTransition();
	// 9) Pyro sense pin
	Serial.println("\nTesting Pyro sense pin");
	pinMode(PYROSENSE, INPUT);
	start_time = millis();
	while (millis() - start_time < interval)
	{
		Serial.println("Pyro sense pin reads: " + String(analogRead(PYROSENSE)));
		delay(400);
	}

	buzzer::signalTransition();
	// 10) Fire pyro pin
	Serial.println("\nTesting Fire pyro pin");
	pinMode(FIREPYRO, OUTPUT);
	start_time = millis();
	while (millis() - start_time < interval)
	{
		digitalWrite(FIREPYRO, HIGH);
		Serial.println("Fire pyro pin set to: HIGH");
		delay(1000);
		digitalWrite(FIREPYRO, LOW);
		Serial.println("Fire pyro pin set to: LOW");
		delay(1000);
	}

	buzzer::signalTransition();
	// 11) GPS
	Serial.println("\nTesting GPS");
	gps::setup();
	while(true)
	{
		gps::readGps();
		delay(400);
	}
}

void loop() {}