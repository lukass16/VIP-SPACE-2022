#include <Arduino.h>
#include "gps_rswrapper.h"
#include "lcd_rswrapper.h"
#include <LoRa.h>
#include <SPI.h>
#include <lcd_rswrapper.h>
#include <lora_rswrapper.h>
#include "sensor_data.h"

sens_data::SensorData s_data;

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

// Packet stuff

int counter = 0;
int lostPackets = 0;
int corruptedPackets = 0;
double successRate = 0;
float receivedRSSI = 0;
float receivedSNR = 0;
int receivedSize = 0;
int mathCounter = 0;
bool startCorruption = 0;
long freqError = 0;

// ScreenSwitching related variables
int LeftSwitch = 25;
int RightSwitch = 26;
short LeftState = 0;
short RightState = 0;
int prevLeftState = 0;
int prevRightState = 0;
bool buttonPressed = 0;

// Screen update logic
int currentScreen = 1;
int prevSats = -1;
int pageCounter = 1;
int prevDisplayedCounter = 0;
int lastCounter = 0; // EXP
bool ScreenSwitched = 0;

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

void calculatePacketInfo()
{
	// Ja divu secīgu pakešu atšķirība ir lielāka par viens, aprēķina izkritušo paketi
	// Ja pienāk nulles pakete pēc tā, ka ir saņemta pakete lielāka par 0, tā tiek uztverta kā koruptēta
	if (lastCounter != counter) // ja ir pienākusi jauna pakete
	{
		if (counter >= 1) // ja nav pienākusi pati pirmā
		{
			lostPackets += counter - lastCounter - 1;						 // aprēķinam cik izkrita paketes
			successRate = 100 - (int)((float)lostPackets / counter * 100.0); // Percentage of good packets from total
			lastCounter = counter;
			Serial.println("Lost Packets: " + String(lostPackets) + " Success rate: " + String(successRate));
		}
		else if (counter == 0 && lastCounter >= 1)
		{
			corruptedPackets++;
		}
	}
}

void setup()
{
	// Testing one by one
	Serial.begin(115200);
	Serial.println("Serial connected");

	// GPS
	Serial.println("Setup GPS");
	gps::setup();

	// LoRa
	Serial.println("Setup LoRa");
	lora::setup();

	// LCD
	Serial.println("Setup LCD");
	lcd::setup();

	// Slēdži
	pinMode(LeftSwitch, INPUT);
	pinMode(RightSwitch, INPUT);
}

void loop()
{
	gps::readGps();

	// Ziņu nolasīšana
	s_data = lora::readEncodedMessage();

	// LoRa parametri
	receivedRSSI = lora::getPacketRssi();
	receivedSNR = lora::getPacketSNR();
	freqError = lora::freqError();

	if (s_data.counter != -1) // ja tika saņemta ziņa
	{
		allotData(s_data);
	}

	// RS GPS aprēķini un funkcijas
	if (lat != 0 && lng != 0)
	{
		distance = gps::getDistance(lat, lng);
		course = gps::getCourseTo(lat, lng);
	}
	sats = gps::getSatellites();
	gpsValid = gps::gpsValid();

	//* SLIKTO PAKEŠU APRĒĶINS
	calculatePacketInfo();

	// Ja pg counter == 2 un ekrāns pārslēgts vai mainās distance vai pienāk jauna pakete, tad atsvaidzina
	// Ja pg counter == 1 un ekrāns pārslēgts vai mainās satelītu skaits, tad atsvaidzina
	// Ja pg counter == 0 un ekrāns pārslēgts vai mainās pakešu skaits, tad atsvaidzina
	// Slēdžu nolasīšana
	LeftState = digitalRead(LeftSwitch);
	RightState = digitalRead(RightSwitch);

	// BUTTON LOGIC
	if (LeftState == 1 && prevLeftState == 0 && pageCounter <= 1)
	{
		pageCounter++;
		prevLeftState = 1;
		buttonPressed = 1;
	}

	if (RightState == 1 && prevRightState == 0 && pageCounter >= 1)
	{
		pageCounter = pageCounter - 1;
		prevRightState = 1;
		buttonPressed = 1;
	}

	buttonPressed = 0;
	prevLeftState = digitalRead(LeftSwitch);
	prevRightState = digitalRead(RightSwitch);

	if (pageCounter == 2 && (currentScreen != 2 || prevDisplayedCounter != counter || prevDistance != distance))
	{
		if (counter >= 1)
		{
			lcd::writeAll(lat, lng, distance, course, bar_alt, f_vel, gpsValid, counter);
			prevDisplayedCounter = counter;
			prevDistance = distance;
			currentScreen = 2;
		}
	}
	else if (pageCounter == 1 && (currentScreen != 1 || prevSats != sats))
	{
		lcd::GPSSetup(sats);
		prevSats = sats;
		currentScreen = 1;
	}
	else if (pageCounter == 0 && (currentScreen != 0 || prevDisplayedCounter != counter))
	{
		lcd::LoRaSetup(lastCounter, lostPackets, successRate, receivedRSSI, receivedSNR, corruptedPackets);
		prevDisplayedCounter = counter;
		currentScreen = 0;
	}
}
