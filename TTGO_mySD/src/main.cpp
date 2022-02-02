//bac ilgi chakareejos un nesapratu kapec nestrada
//pilnigi automatiski iedomajos, ka SD javieno pie 3.3V
//pievieno SD pie 5V ..viss perfekti ..<3

//main source                 https://github.com/LilyGO/TTGO-T-Beam/issues/9 
//shis mani apgaismoja par 5V https://forum.arduino.cc/t/problem-with-spi-on-ttgo-t-beam-for-microsd/677767 
//GPS telefonaa! Soo coool    https://www.instructables.com/Android-TTGO-T-Beam-GPS-Tracker/

#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <mySD.h>

//#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <TinyGPS++.h> // Renamed so to use a NEO M6N GPS - There's also another library available modified by Andrea Spiess for the NEO M8N GPS
//#include <BMP280.h>

// Define the chip select GPIO for the SD Card - T-Beam
#define SD_cs 25 // goes to the CS pin on the SD card reader

// Define an instance for the file for the SD card
File myFile;

// Define the LoRa module GPIO pins - these are the default GPIO's that are actually connected to LoRa module on the T-Beam
#define LoRa_SCK 5 // GPIO5 -- SX1278's SCK
#define LoRa_MISO 19 // GPIO19 -- SX1278's MISO - may not be needed for the TFT but needed for the SD card reader
#define LoRa_MOSI 27 // GPIO27 -- SX1278's MOSI
#define LoRa_CS 18 // GPIO18 -- SX1278's CS
#define LoRa_RST 23 // GPIO14 -- SX1278's RESET
#define LoRa_DI0 26 // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define Lora_BAND 868E6 // Frequency 868mHz - change to whichever frequency you are using

// Define the variables for the received LoRa packet
String rssi = "RSSI --"; // Only if you need these
String packSize = "--";
String packet;
uint32_t packetSize;
int32_t packetsReceived = 0;

// Define the GPIO's for the TFT - T-Beam
#define _cs 2 // goes to TFT CS
#define _dc 32 // goes to TFT DC
#define _mosi 13 // goes to TFT MOSI
#define _sclk 14 // goes to TFT SCK/CLK
#define _rst 33 // goes to TFT RESET
#define _miso 4 // may not be necessary to connect to the TFT
// 3.3V // goes to TFT LED
// 5v // goes to TFT Vcc
// Gnd // goes to TFT Gnd

// Uses software SPI
Adafruit_ILI9341 tft = Adafruit_ILI9341(_cs, _dc, _mosi, _sclk, _rst);

// Define GPIO's for GPS & baud rate
static const int GPS_RX = 12, GPS_TX = 15; // NOTE - The TX and RX pins are marked incorrectly on the T-Beam !!
static const uint32_t GPSBaud = 9600;

// Define an instance of the TinyGPS++ object
TinyGPSPlus GPS;

// The serial connection to the GPS device
SoftwareSerial ss(GPS_RX, GPS_TX);

//.
//.
//.

void setup(){
// Start the serial port for debugging
Serial.begin(115200);
// Initialize SD Card
pinMode(SS, OUTPUT);

if (SD.begin(SD_cs, _mosi, _miso, _sclk)) {
Serial.println("SD card initialized OK...");}
else{
Serial.println("SD card initialization FAILED...");}

// If the test.txt file doesn't exist
// create the file on the SD card and write to it
myFile = SD.open("test.txt", FILE_WRITE);

// If the file opened okay, write to it:
if (myFile)
{
  Serial.print("Writing to test.txt...");
  myFile.println("Hacking......................................................");
  myFile.println("Hacking......................................................");
  myFile.println("Hacking......................................................");
  myFile.println("Hello, comrad");
  myFile.close();
  Serial.println("Done.");      
}

 myFile = SD.open("test.txt"); // Re-open the file for reading:
 if (myFile){
   Serial.println("test.txt:");
   
   while (myFile.available()){ // Read from the file until there's nothing else in it:
     Serial.write(myFile.read());
     delay (20);}
   
   myFile.close(); // close the file:
 }

//Wire.begin(BMP280_SDA, BMP280_SCL);
//BMP280.begin(); // initialise BMP280 for continuous measurements


ss.begin(GPSBaud); // Start GPS Services

while (ss.available() > 0)
GPS.encode(ss.read());

Serial.print("Latitude : ");
Serial.println(GPS.location.lat(), 5);
Serial.print("Longitude : ");
Serial.println(GPS.location.lng(), 4);
Serial.print("Satellites: ");
Serial.println(GPS.satellites.value());
Serial.print("Altitude : ");
Serial.print(GPS.altitude.feet() / 3.2808);
Serial.println("M");
Serial.print("Time : ");
Serial.print(GPS.time.hour());
Serial.print(":");
Serial.print(GPS.time.minute());
Serial.print(":");
Serial.println(GPS.time.second());
Serial.println("**********************");

delay(1000);

if (millis() > 5000 &&GPS.charsProcessed() < 10){
Serial.println(F("No GPS data received: check wiring"));
}

LoRa.setPins(LoRa_CS, LoRa_RST, LoRa_DI0); // Start the LoRa service

if (!LoRa.begin(Lora_BAND)){
Serial.println("Starting LoRa failed...");
while (1); // HALT}
Serial.println("LoRA initilazed OK..."); // Put LoRa module into receive mode
LoRa.receive();
}
}

void loop(){

}