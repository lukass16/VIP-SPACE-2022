#include <Arduino.h>
#include <Kalman1.h>
#include <heltec.h>
#include <SPIFFS.h>
#include <SD.h>
//http://blog.tkjelectronics.dk/2012/09/a-practical-approach-to-kalman-filter-and-how-to-implement-it/
//Slikts nosakums, bet šis ir HELTEC
//iedvesma:	default esp32 SD sample code no libary ESP-FTP-Server-Lib
//			https://github.com/LilyGO/TTGO-T-Beam/issues/9

//thread par LoRa un SD konfliktu: https://stackoverflow.com/questions/57454066/how-to-use-2-spi-devices-lora-and-sd-card-on-esp32

#define _miso 4
#define _mosi 13 
#define _sclk 14
#define SD_cs 25

void initializeSD (){
  SPIFFS.begin(true);
	SPI.begin(_sclk, _miso, _mosi, SD_cs);

	if(!SD.begin(SD_cs))
	{
		Serial.println("SD Card Mount Failed");
	}
	else Serial.println("SD connected");
}

void DoData(File write, File read){
  String line = "";
  char c='0';
  int i=1;
  if (read){
    while (read.available()){
      while (read.available()){ // Read from the file until there's nothing else in it:
                { // Read from the file until a newline character is encountered:
                    char c = read.read();
                    if (c == ',')break;
                    line += (char)c;
                }
      }    
          write.println(KALMAN((line.toDouble())));
          Serial.println(line); 
          line = ""; 
    }
      }
}

