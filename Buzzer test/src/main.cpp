#include <Arduino.h>
#include "buzzer.h"

void setup()
{
	Serial.begin(115200);
	Serial.println("Buzzer test started");
	buzzer::setup();

	Serial.println("Signal start");
	buzzer::signalStart();
	delay(3000);

	Serial.println("Signal Calibration start");
	buzzer::signalCalibrationStart();
	delay(3000);

	Serial.println("Signal Calibration end");
	buzzer::signalCalibrationEnd();
	delay(3000);

	unsigned long start_time = millis();

	while (millis() - start_time < 10000)
	{
		Serial.println("Signal Second Switch");
		buzzer::signalSecondSwitch();
	}
	Serial.println("Signal Second Switch");
	buzzer::signalStart();
	delay(3000);
	buzzer::buzzEnd();
	buzzer::transitionToGeneratorMode();
	Serial.println("Transitioned to Generator Mode");
	delay(2000);
}

void loop()
{
	buzzer::signalFlight();
}