#include <Arduino.h>
//https://randomnerdtutorials.com/esp32-plot-readings-charts-multiple/

//These are not in the .ino file!!!!!!!!!!!!!!!!!!!!!!

//https://github.com/me-no-dev/AsyncTCP
//https://github.com/me-no-dev/ESPAsyncWebServer

/*********
  Rui Santos
  Complete instructions at https://RandomNerdTutorials.com/esp32-plot-readings-charts-multiple/

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

#include <LoRa.h>
#include <SPI.h>
#include <lora_rswrapper.h>
#include "flash_rswrapper.h"
#include "sensor_data.h"
//#include "FS.h"
#include "SPIFFS.h"
#include "LITTLEFS.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h" //https://randomnerdtutorials.com/esp32-vs-code-platformio-spiffs/
                    //https://www.youtube.com/watch?v=Pxpg9eZLoBI 
#include "FS.h"
#include <Arduino_JSON.h>

// Replace with your network credentials
const char *ssid = "Monjiik phone";
const char *password = "pelmenis";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create an Event Source on /events
AsyncEventSource events("/events");

// Json Variable to Hold Sensor Readings
JSONVar readings;

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 3000;

// GPIO where the DS18B20 sensors are connected to
//const int oneWireBus = 4;

// Setup a oneWire instance to communicate with OneWire devices (DS18B20)
//OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor
//DallasTemperature sensors(&oneWire);

// Address of each sensor
//DeviceAddress sensor3 = { 0x28, 0xFF, 0xA0, 0x11, 0x33, 0x17, 0x3, 0x96 };
//DeviceAddress sensor1 = { 0x28, 0xFF, 0xB4, 0x6, 0x33, 0x17, 0x3, 0x4B };
//DeviceAddress sensor2 = { 0x28, 0xFF, 0x43, 0xF5, 0x32, 0x18, 0x2, 0xA8 };
//DeviceAddress sensor4 = { 0x28, 0xFF, 0x11, 0x28, 0x33, 0x18, 0x1, 0x6B };

// Get Sensor Readings and return JSON object
String getSensorReadings(int bar_alt, int f_vel){
  //sensors.requestTemperatures();
  readings["sensor1"] = bar_alt;
  readings["sensor2"] = f_vel;
  readings["sensor3"] = 2;
  readings["sensor4"] = 3;

  String jsonString = JSON.stringify(readings);
  return jsonString;
}

// Initialize SPIFFS
void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  else{
    Serial.println("SPIFFS mounted successfully");
  }
}

// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}



sens_data::SensorData s_data;

// defining message string and deserialization buffer
String message = "";
char buffer[80] = "";

// defining receivable sensor variables
double lat = 0;
double lng = 0;
double alt = 0;

float acc_x = 0;
float acc_y = 0;
float acc_z = 0;

float pres = 0;
float bar_alt = 0;
float f_alt = 0;
float f_vel = 0;
float bat1;

int r_state = 0;
int counter = 0;


// Flash writing logic
bool holdBoth = 0;
unsigned long start_hold = millis();
unsigned long hold_time = 0;
bool canWriteToFlash = 0;
bool prevCanWriteToFlash = 0;
bool newPacket = 0;
File file;


// defining necessary functional variables
double prevDistance = 0;
double distance = 0;
double course = 0;
double sats = 0;
bool gpsValid = 0;

void allotData(sens_data::SensorData s_data)
{
	// GPS data
	lat = s_data.gpsData.lat;
	lng = s_data.gpsData.lng;
	alt = s_data.gpsData.alt;
	sats = s_data.gpsData.sats;

	// IMU data
	acc_x = s_data.imuData.acc_x;
	acc_y = s_data.imuData.acc_y;
	acc_z = s_data.imuData.acc_z;

	// Barometer data
	pres = s_data.barometerData.pressure;
	bar_alt = s_data.barometerData.altitude;
	f_alt = s_data.barometerData.f_altitude;
	f_vel = s_data.barometerData.f_velocity;

	// Battery data
	bat1 = s_data.batteryData.bat1;

	// Other data
	r_state = s_data.current_rocket_state;
	counter = s_data.counter;
}

void printData()
{
	Serial.printf("%7.4f,%7.4f,%5.0f,%2d,%4.2f,%4.2f,%4.2f,%5.0f,%6.1f,%6.1f,%3.0f,%2.1f,%1d,%4d\n", lat, lng, alt, sats, acc_x, acc_y, acc_z, pres, bar_alt, f_alt, f_vel, bat1, r_state, counter);
}

void deserializeData(char buffer[])
{
	sscanf(buffer, "%lf,%lf,%lf,%d,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d", &lat, &lng, &alt, &sats, &acc_x, &acc_y, &acc_z, &pres, &bar_alt, &f_alt, &f_vel, &bat1, &r_state, &counter); // for deserialization double ahs to vbe specified as %lf
}



void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println("Ongoing tests");
  initWiFi();
  initSPIFFS();

  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.serveStatic("/", SPIFFS, "/");

  // Request for the latest sensor readings
  server.on("/readings", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = getSensorReadings(0,0);
    request->send(200, "application/json", json);
    json = String();
  });


    events.onConnect([](AsyncEventSourceClient *client){
     Serial.println("Ongoing tests5");
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
   
  });
  server.addHandler(&events);


  

	Serial.println("Serial connected");

	// LoRa
	Serial.println("Setup LoRa");
	lora::setup();

	// Flash
	//flash::setup();

  // Start server
  server.begin();
}

void loop() {

  if ((millis() - lastTime) > timerDelay) {
    // Send Events to the client with the Sensor Readings Every 10 seconds
    events.send("ping",NULL,millis());
    events.send(getSensorReadings(bar_alt, f_vel).c_str(),"new_readings" ,millis());
    lastTime = millis();
  }

  
	s_data = lora::readEncodedMessage();
	if (s_data.counter != -1) // ja tika saņemta ziņa
	{
		allotData(s_data);
	}

	// LoRa parametri
	//receivedRSSI = lora::getPacketRssi();
	//receivedSNR = lora::getPacketSNR();
	//freqError = lora::freqError();


	// if (newPacket) //ja pienākusi jauna - rakstam iekšā flash;
	// {
	// 	flash::writeData(file, canWriteToFlash, lat, lng, alt, sats, acc_x, acc_y, acc_z, pres, bar_alt, f_alt, f_vel, bat1, r_state, counter); //ja nav atļauts 
	// }
}



