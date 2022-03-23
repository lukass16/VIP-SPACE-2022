#include <SPIFFS.h>
#include <mySD.h>
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
  pinMode(SD_cs, OUTPUT); //SS

  if (SD.begin(SD_cs, _mosi, _miso, _sclk)) {
    Serial.println("SD card initialized OK...");}
  else{
    Serial.println("SD card initialization FAILED...");}

}

void readSD(SD_File read){
    if (read) while (read.available()) Serial.write(read.read());
    read.close();
}     


void DoData(SD_File write, SD_File read){
  String line = "";
  
  if (read){
    while (read.available()){
      while (read.available()){ // Read from the file until there's nothing else in it:
                { // Read from the file until a newline character is encountered:
                    char c = read.read();
                    if (c == ',')break;
                    line += (char)c;
                }
      }    
          write.println((line.toDouble()));
          Serial.println(line); 
          line = ""; 
    }
      }
}