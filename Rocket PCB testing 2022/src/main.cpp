#include <Arduino.h>
#include "buzzer.h"
#include "lora_wrapper.h"

unsigned long start_time = millis();
int interval = 10000; //milliseconds

void setup()
{
	Serial.begin(115200);

	//1) Buzzer
	buzzer::setup();
	buzzer::signalTestStart();
	buzzer::transitionToGeneratorMode();

	buzzer::signalTransition();
	//2) LoRa
	lora::setup();
	start_time = millis();
	String message = "TESTING LORA";
	while(millis() - start_time < interval)
	{
		lora::sendMessage(message, 1);
		delay(400);
	}

	buzzer::signalTransition();
	//3) Barometer
	while(true);


}

void loop()
{
	// put your main code here, to run repeatedly:
}