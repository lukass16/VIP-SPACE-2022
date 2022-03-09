#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <heltec.h>
#include <SPIFFS.h>
#include <mySD.h>


#include <flash.h>
#include <SD_card.h> //zinu, ka tu dievini faila nosaukumus ar ___________ ;D



void setup(){
    Serial.begin(115200);

    initializeSD();
    initializeFlash();
    

    deleteFile(LITTLEFS, "/test.txt");

    File writeFlash = LITTLEFS.open("/test.txt", FILE_APPEND); //prieksh flash
    Files writeSD = SD.open("test.txt", FILE_WRITES); //prieksh SD

    uint32_t start = millis();
    for(int i=0; i<10; i++){
        testFileIO(writeFlash, i);
        writeSD.println(i);
    }
    uint32_t end = millis() - start;
    writeFlash.close();
    writeSD.close();


    Serial.println("Flash contents: ");
    readFlash(LITTLEFS, "/test.txt");
    Serial.println("SD contents: ");
    Files readFile = SD.open("test.txt");
    readSD(readFile);
    

    Serial.printf("bytes written in %u ms\r\n", end);
    Serial.println( "Test complete" );

}


void loop(){

}
