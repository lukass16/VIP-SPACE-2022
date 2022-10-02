#pragma once
#include <SPIFFS.h>
#include <mySD.h>
#include "sensor_data.h"

// http://blog.tkjelectronics.dk/2012/09/a-practical-approach-to-kalman-filter-and-how-to-implement-it/
// Inspiration:	default esp32 SD sample code from library ESP-FTP-Server-Lib
//https://github.com/LilyGO/TTGO-T-Beam/issues/9

// thread about LoRa and SD conflict: https://stackoverflow.com/questions/57454066/how-to-use-2-spi-devices-lora-and-sd-card-on-esp32

#define _MISO 13
#define _MOSI 12
#define _SCLK 2
#define _SD_CS 25

unsigned long SD_time = millis();

namespace SDcard
{

	void setup()
	{
		pinMode(_SD_CS, OUTPUT);

		if (SD.begin(_SD_CS, _MOSI, _MISO, _SCLK))
		{
			Serial.println("SD card initialized OK");
		}
		else
		{
			Serial.println("SD card initialization FAILED");
		}
	}

	SD_File openFile() // returns file handle
	{
		SD_File fileSD = SD.open("data.txt", FILE_WRITES);
		return fileSD;
	}

	SD_File openNextFile() // returns file handle - opens iteratively
	{
		char filename[7] = "0.csv";
		int it = 0;
		String tester;

		while(SD.exists(filename)) // while there already exists a file with such a name
		{
			it++;
			tester = String(it) + ".csv";
			tester.toCharArray(filename, 7);
		}

		Serial.println("Opening file: " + String(filename));

		SD_File fileSD = SD.open(filename, FILE_WRITES);
		return fileSD;
	}

	void closeFile(SD_File fileSD)
	{
		fileSD.close();
	}

	SD_File reloadFile(SD_File fileSD) // returnds file handle
	{
		Serial.println("Reloading SD card file");
		closeFile(fileSD);
		return SD.open("data.txt", FILE_WRITES);
	}

	float getTimeElapsed()
	{
		return millis() - SD_time;
	}

	int writeString(SD_File fileSD, String serialized)
	{
		if (!fileSD)
		{
			Serial.println("Unable to write to SD card");
			return 0;
		}
		fileSD.println(serialized);
		return 1;
	}

	int writeHeader(SD_File fileSD)
	{
		if (!fileSD)
		{
			Serial.println("Unable to write to SD card");
			return 0;
		}

		fileSD.println("Time [ms],Lat,Lng,GPS Alt [m],Sats,Pressure [hPa],Raw Baro Alt [m],Filter Baro Alt [m],Baro Vert Vel [m/s],Baro Vert Acc [m/s^2],Baro Temp [°C],Bat 1 Voltage [V],IMU Acc X [m/s^2],IMU Acc Y [m/s^2],IMU Acc Z [m/s^2],IMU Gyr X [deg/s],IMU Gyr Y [deg/s],IMU Gyr Z [deg/s],State");
		return 1;
	}

	void writeData(SD_File fileSD, float time, float lat, float lng, float alt, float sats, float pressure, float altitude, float f_altitude, float f_velocity, float f_acceleration, float temperature, float bat1, float acc_x, float acc_y, float acc_z, float gyr_x, float gyr_y, float gyr_z, float rState)
	{
		if (fileSD)
		{
			fileSD.println(String(time, 2) + "," + String(lat, 4) + "," + String(lng, 4) + "," + String(alt, 2) + "," + String(sats) + "," + String(pressure, 2) + "," + String(altitude, 2) + "," + String(f_altitude, 2) + "," + String(f_velocity, 2) + "," + String(f_acceleration, 2) + "," + String(temperature, 1) + "," + String(bat1, 2) + "," + String(acc_x, 2) + "," + String(acc_y, 2) + "," + String(acc_z, 2) + "," + String(gyr_x, 2) + "," + String(gyr_y, 2) + "," + String(gyr_z, 2) + "," + String(rState, 0));
		}
	}

	int writeDataStruct(SD_File fileSD, float time, sens_data::GpsData gpsData, sens_data::IMUData imuData, sens_data::BarometerData barData, sens_data::BatteryData batData, bool verbose = false)
	{
		if (!fileSD)
		{
			Serial.println("Unable to write to SD card");
			return 0;
		}

		// GPS
		float lat = gpsData.lat;   // 1.1
		float lng = gpsData.lng;   // 1.2
		float alt = gpsData.alt;   // 1.3
		float sats = gpsData.sats; // 1.4

		// Bar
		float pressure = barData.pressure;			   // 2.1
		float altitude = barData.altitude;			   // 2.2
		float f_altitude = barData.f_altitude;		   // 2.3
		float f_velocity = barData.f_velocity;		   // 2.4
		float f_acceleration = barData.f_acceleration; // 2.5
		float temperature = barData.temperature;	   // 2.6

		// Bat
		float bat1 = batData.bat1; // 3.1
		float bat2 = batData.bat2; // 3.2

		// Mag
		float mag_x = imuData.mag_x; // 4.1
		float mag_y = imuData.mag_y; // 4.2
		float mag_z = imuData.mag_z; // 4.3
		float acc_x = imuData.acc_x; // 4.4
		float acc_y = imuData.acc_y; // 4.5
		float acc_z = imuData.acc_z; // 4.6

		if (verbose)
		{
			Serial.println(String(time, 2) + "," + String(lat, 4) + "," + String(lng, 4) + "," + String(alt, 2) + "," + String(sats) + "," + String(pressure, 2) + "," + String(altitude, 2) + "," + String(f_altitude, 2) + "," + String(f_velocity, 2) + "," + String(f_acceleration, 2) + "," + String(temperature, 1) + "," + String(bat1, 2) + "," + String(bat2, 2) + "," + String(mag_x, 2) + "," + String(mag_y, 2) + "," + String(mag_z, 2) + "," + String(acc_x, 2) + "," + String(acc_y, 2) + "," + String(acc_z, 2));
		}

		fileSD.println(String(time, 2) + "," + String(lat, 4) + "," + String(lng, 4) + "," + String(alt, 2) + "," + String(sats) + "," + String(pressure, 2) + "," + String(altitude, 2) + "," + String(f_altitude, 2) + "," + String(f_velocity, 2) + "," + String(f_acceleration, 2) + "," + String(temperature, 1) + "," + String(bat1, 2) + "," + String(bat2, 2) + "," + String(mag_x, 2) + "," + String(mag_y, 2) + "," + String(mag_z, 2) + "," + String(acc_x, 2) + "," + String(acc_y, 2) + "," + String(acc_z, 2));
		return 1;
	}

	void markPreparation(SD_File fileSD)
	{
		fileSD.println("PREPARATION STATE");
	}

	void markDrogue(SD_File fileSD)
	{
		fileSD.println("DROGUE STATE");
	}

	void markMain(SD_File fileSD)
	{
		fileSD.println("MAIN STATE");
	}

	void markDescent(SD_File fileSD)
	{
		fileSD.println("DESCENT STATE");
	}

	void readFullSD(SD_File rfile)
	{
		if (rfile)
		{
			while (rfile.available())
			{
				Serial.write(rfile.read());
			}
		}
		else
		{
			Serial.println("Cannot read SD file");
		}
		rfile.close();
	}

	void test(SD_File fileSD)
	{
		fileSD.println("Testing write SD card");
	}

};