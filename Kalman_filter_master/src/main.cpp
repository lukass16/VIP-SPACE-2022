#include <Arduino.h>
#include <Kalman1.h>
#include <heltec.h>
#include <SPIFFS.h>
#include <SD.h>
#include <SD_code.h>
//http://blog.tkjelectronics.dk/2012/09/a-practical-approach-to-kalman-filter-and-how-to-implement-it/
//Slikts nosakums, bet šis ir HELTEC
//iedvesma:	default esp32 SD sample code no libary ESP-FTP-Server-Lib
//			https://github.com/LilyGO/TTGO-T-Beam/issues/9

//thread par LoRa un SD konfliktu: https://stackoverflow.com/questions/57454066/how-to-use-2-spi-devices-lora-and-sd-card-on-esp32

File readFile, writeFile;


void setup()
{
  Serial.begin(115200);
	Serial.println("Heltec test");
	initializeSD();


  readFile = SD.open("/raw.txt"); // Re-open the file for reading:
  writeFile = SD.open("/result.txt", FILE_WRITE); //šis for some reason iztīra failu pirms rakstīšanas, idk jāpadomā, TTGO gan vnk raksta klāt

  DoData(writeFile, readFile);
  
  readFile.close(); // close the file:
  writeFile.close(); 
   

 }

void loop(){
}
