#include <Arduino.h>
#include <wifiserver.h>

void setup()
{
	Serial.begin(115200);
	Serial.println("Starting Wi-Fi Soft AP test");
	wifiserver::setup();
}

void loop()
{
	wifiserver::handleClient();
}