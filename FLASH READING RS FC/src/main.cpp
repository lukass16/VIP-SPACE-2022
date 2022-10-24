#include <Arduino.h>
#include "flash.h"
#include "buzzer.h"

void setup()
{
	Serial.begin(115200);

	//*Buzzer setup
	buzzer::setup();

	//*Flash setup
	flash::setup();

	//*Dump contents
	buzzer::signalFlashStart();
	flash::dumpContentsToSerial("/data.txt");
	buzzer::signalFlashEnd();
}

void loop() {}