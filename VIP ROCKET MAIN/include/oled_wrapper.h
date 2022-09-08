#pragma once
#include <U8g2lib.h>

//https://github.com/m1k0s/ESP32-HelloWorld3D/blob/master/include/display.h

//define oled object
U8X8_SSD1306_128X64_NONAME_SW_I2C screen(15, 4, 16);

namespace oled
{
    void setup()
    {
        screen.begin();
	    screen.setFont(u8x8_font_chroma48medium8_r);
        screen.drawString(1, 3, "Prep");
        Serial.println("OLED SETUP");
    }

    void print(char buffer[])
    {
        screen.drawString(1, 3, buffer);
    }

};