//Slikts nosakums, bet šis ir HELTEC
//iedvesma:	default esp32 SD sample code no libary ESP-FTP-Server-Lib
//			https://github.com/LilyGO/TTGO-T-Beam/issues/9

//thread par LoRa un SD konfliktu: https://stackoverflow.com/questions/57454066/how-to-use-2-spi-devices-lora-and-sd-card-on-esp32

#include <Arduino.h>
#include <heltec.h>
#include <SPIFFS.h>
#include <SD.h>

//cik saprotu esp32 laikam var izvēlēties arbitrary SPI pins
//bet ar pilnīgi random pins sadly nestrādāja 
//bet šie ir tādi kā TTGO un nepārklājas ar LoRa

#define _miso 4
#define _mosi 13 
#define _sclk 14
#define SD_cs 25

File myFile;
void setup()
{
  	Serial.begin(115200);
	Serial.println("Heltec test");
	SPIFFS.begin(true);
	//SPI.begin(5, 19, 27); //šie bija orģināli kodā, neko nesaprotu par LoRa, bet nu sakrīt pini
	//if(!SD.begin(21))
	SPI.begin(_sclk, _miso, _mosi, SD_cs);
	if(!SD.begin(SD_cs))
	{
		Serial.println("SD Card Mount Failed");
	}
	else Serial.println("SD connected");

	myFile = SD.open("/test.txt", FILE_WRITE); //šis for some reason iztīra failu pirms rakstīšanas, idk jāpadomā, TTGO gan vnk raksta klāt

// If the file opened okay, write to it:
if (myFile)
{
  Serial.print("Writing to test.txt...");
  myFile.println("Hacking......................................................");
  myFile.println("Hacking......................................................");
  myFile.println("Hacking......................................................");
  myFile.println("Arvīds ir Igauņu spiegs!");
  myFile.close();
  Serial.println("Done.");      
}

 myFile = SD.open("/test.txt"); // Re-open the file for reading:
 if (myFile){
   Serial.println("test.txt:");
   
   while (myFile.available()){ // Read from the file until there's nothing else in it:
     Serial.write(myFile.read());
	 delay(50);}
   
   myFile.close(); // close the file:
 }

}
	

void loop(){
}
