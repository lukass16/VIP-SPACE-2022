#include <Arduino.h>
#include "gps_rswrapper.h"
#include "lcd_rswrapper.h"
#include <LoRa.h>
#include <SPI.h>
#include <lcd_rswrapper.h>
#include <lora_rswrapper.h>
#include "flash_rswrapper.h"
#include "sensor_data.h"
#include "FS.h"

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

// Packet stuff

int counter = 0;
int lostPackets = 0;
int corruptedPackets = 0;
double successRate = 0;
float receivedRSSI = 0;
float receivedSNR = 0;
int receivedSize = 0;
long freqError = 0;

// ScreenSwitching related variables
int LeftSwitch = 25;
int RightSwitch = 26;
short LeftState = 0;
short RightState = 0;
int prevLeftState = 0;
int prevRightState = 0;
bool buttonPressed = 0;

// Flash writing logic
bool holdBoth = 0;
unsigned long start_hold = millis();
unsigned long hold_time = 0;
bool canWriteToFlash = 0;
bool prevCanWriteToFlash = 0;
bool newPacket = 0;
File file;

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

void deserializeData(char buffer[])
{
	sscanf(buffer, "%lf,%lf,%lf,%d,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d", &lat, &lng, &alt, &sats, &acc_x, &acc_y, &acc_z, &pres, &bar_alt, &f_alt, &f_vel, &bat1, &r_state, &counter); // for deserialization double ahs to vbe specified as %lf
}

bool calculatePacketInfo() // atgriež true, ja pienākusi jauna pakete, false, ja nē
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
			// Serial.println("Lost Packets: " + String(lostPackets) + " Success rate: " + String(successRate));
		}
		else if (counter == 0 && lastCounter >= 1) // ja rāda 0, bet nav pirmā
		{
			corruptedPackets++;
		}
		return true; //ir pienākusi jauna
	}
	return false; //nav pienākusi jauna
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

	// Flash
	gps::disableSerial();
	flash::setup();
	gps::enableSerial();

	// Slēdži
	pinMode(LeftSwitch, INPUT);
	pinMode(RightSwitch, INPUT);
}

void loop()
{
	// lasām gps
	gps::readGps();

	// Ziņu nolasīšana

	//*option 1 - Not encoded
	message = lora::readMessage();
	message.toCharArray(buffer, 80);
	deserializeData(buffer);

	//*option 2 - Encoded
	// s_data = lora::readEncodedMessage();
	// if (s_data.counter != -1) // ja tika saņemta ziņa
	// {
	// 	allotData(s_data);
	// }

	// LoRa parametri
	receivedRSSI = lora::getPacketRssi();
	receivedSNR = lora::getPacketSNR();
	freqError = lora::freqError();

	// RS GPS aprēķini un funkcijas
	if (lat != 0 && lng != 0)
	{
		distance = gps::getDistance(lat, lng);
		course = gps::getCourseTo(lat, lng);
	}
	sats = gps::getSatellites();
	gpsValid = gps::gpsValid();

	//

	//*Slikto pakešu aprēķins
	newPacket = calculatePacketInfo(); //atgriež vai pienākusi jauna pakete, un aprēķina paketes kvalitātes info

	if (newPacket) //ja pienākusi jauna - rakstam iekšā flash;
	{
		gps::disableSerial();
		flash::writeData(file, canWriteToFlash, lat, lng, alt, sats, acc_x, acc_y, acc_z, pres, bar_alt, f_alt, f_vel, bat1, r_state, counter); //ja nav atļauts 
		gps::enableSerial();
	}

	//*Slēdžu nolasīšana
	LeftState = digitalRead(LeftSwitch);
	RightState = digitalRead(RightSwitch);

	//*Nosakām vai tiek turētas abas pogas kopā
	if (LeftState == 0 && RightState == 0 && !holdBoth) // ja nospiestas kopā pirmo reizi
	{
		holdBoth = 1;
		start_hold = millis(); // sākam timeri
	}
	else if (LeftState == 0 && RightState == 0 && holdBoth) // ja jau bija nospiestas abas un joprojām ir
	{
		hold_time = millis() - start_hold;
	}
	else if (holdBoth) // ja bija, bet vairs nav
	{
		holdBoth = 0;
	}
	else // ja vairs nav
	{
		hold_time = 0; // piezīme: neatstatam uzreiz, lai vienu reizi sanāktu pārbaudīt vai pārsniegts laiks un atlaists
	}

	//*Rīkojamies, ja pārsniegts intervāls cik turējām līdz atlaidām
	if (hold_time > 15000 && !holdBoth) // ja turējām vairāk par 15 sekundēm un atlaidām
	{
		Serial.println("Turējām vairāk par 15!");
		gps::disableSerial();
		flash::deleteFile("/test.txt");
		gps::enableSerial();
	}
	else if (hold_time > 10000 && !holdBoth) // ja turējām vairāk par 10 sekundēm un atlaidām
	{
		gps::disableSerial();
		flash::readFlashVerbose("/test.txt");
		gps::enableSerial();
		Serial.println("Turējām vairāk par 10!");
	}
	else if (hold_time > 3000 && !holdBoth) // ja turējām vairāk par 3 sekundēm un atlaidām
	{
		Serial.println("Turējām vairāk par 3!");
		if (!canWriteToFlash) // atveram flash
		{
			gps::disableSerial();
			file = flash::openFile();
			gps::enableSerial();
			canWriteToFlash = 1;
		}
		else // ja atkārtoti - tad aizveram flash
		{
			gps::disableSerial();
			flash::closeFile(file);
			canWriteToFlash = 0;
			gps::enableSerial();
		}
	}

	//*Nosakām nospiestu pogu individuālo stāvokli
	if (LeftState == 1 && prevLeftState == 0 && pageCounter <= 1) // ja konstatējam atlaistu kreiso un ja varam tad pabraucam pa kreisi
	{
		pageCounter++;
	}
	prevLeftState = LeftState; // jaunais paliek par veco

	if (RightState == 1 && prevRightState == 0 && pageCounter >= 1) // ja konstatējam atlaistu labo un ja varam tad pabraucam pa labi
	{
		pageCounter--;
	}
	prevRightState = RightState; // jaunais paliek par veco

	//*Pārbaudam vai vajag atjaunot ekrānu
	// Ja pg counter == 2 un ekrāns pārslēgts vai mainās distance vai pienāk jauna pakete, tad atsvaidzina
	if (pageCounter == 2 && (currentScreen != 2 || prevDisplayedCounter != counter || prevDistance != distance))
	{
		if (counter >= 1 || lastCounter == 0) // ja pakete nav koruptēta vai ja vēl nav neviena pakete pienākusi
		{
			lcd::showIncomingData(lat, lng, distance, course, bar_alt, f_vel, r_state, counter);
			prevDisplayedCounter = counter;
			prevDistance = distance;
			currentScreen = 2;
		}
	}

	// Ja pg counter == 1 un ekrāns pārslēgts vai mainās satelītu skaits, tad atsvaidzina
	else if (pageCounter == 1 && (currentScreen != 1 || prevSats != sats || canWriteToFlash != prevCanWriteToFlash))
	{
		lcd::showRSinfo(sats, gpsValid, canWriteToFlash);
		prevSats = sats;
		prevCanWriteToFlash = canWriteToFlash;
		currentScreen = 1;
	}

	// Ja pg counter == 0 un ekrāns pārslēgts vai mainās pakešu skaits, tad atsvaidzina
	else if (pageCounter == 0 && (currentScreen != 0 || prevDisplayedCounter != counter))
	{
		lcd::showLoRaInfo(lastCounter, lostPackets, successRate, receivedRSSI, receivedSNR, corruptedPackets);
		prevDisplayedCounter = counter;
		currentScreen = 0;
	}
}
