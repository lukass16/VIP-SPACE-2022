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

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h" //https://randomnerdtutorials.com/esp32-vs-code-platformio-spiffs/
                    //https://www.youtube.com/watch?v=Pxpg9eZLoBI 
#include "FS.h"
#include <Arduino_JSON.h>

// Replace with your network credentials
const char *ssid = "HUAWEI-9EFF";
const char *password = "39016644";

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
String getSensorReadings(){
  //sensors.requestTemperatures();
  readings["sensor1"] = random(30);
  readings["sensor2"] = 1;
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
    String json = getSensorReadings();
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


    // Start server
  server.begin();
}

void loop() {

  if ((millis() - lastTime) > timerDelay) {
    // Send Events to the client with the Sensor Readings Every 10 seconds
    events.send("ping",NULL,millis());
    events.send(getSensorReadings().c_str(),"new_readings" ,millis());
    lastTime = millis();
  }
}
