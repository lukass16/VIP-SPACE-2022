#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <WiFi.h>
#include "barometer_wrapper.h"

//some links used
//https://github.com/m1k0s/ESP32-HelloWorld3D/blob/master/include/display.h
//https://randomnerdtutorials.com/esp32-web-server-arduino-ide/

//define oled object
U8X8_SSD1306_128X64_NONAME_SW_I2C oled(15, 4, 16);

void setup()
{
	Serial.begin(115200);
	Serial.println("Heltec test");

	Wire.begin(21, 22);
	barometer::setup();

	//oled setup
	oled.begin();
	oled.setFont(u8x8_font_chroma48medium8_r);
	oled.drawString(1, 3, "Shalom");
}

void loop()
{
	barometer::printSensorValues();
}
