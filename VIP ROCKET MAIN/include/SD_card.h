#include <SPIFFS.h>
#include <mySD.h>
#include "sensor_data.h"

// http://blog.tkjelectronics.dk/2012/09/a-practical-approach-to-kalman-filter-and-how-to-implement-it/
// Slikts nosakums, bet šis ir HELTEC
// iedvesma:	default esp32 SD sample code no libary ESP-FTP-Server-Lib
//			https://github.com/LilyGO/TTGO-T-Beam/issues/9

// thread par LoRa un SD konfliktu: https://stackoverflow.com/questions/57454066/how-to-use-2-spi-devices-lora-and-sd-card-on-esp32

#define _MISO 13
#define _MOSI 12
#define _SCLK 2
#define _SD_CS 25

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

	SD_File openFile() //returnds file handle
	{
		SD_File fileSD = SD.open("data0.txt", FILE_WRITES);
		return fileSD;
	}

	void closeFile(SD_File fileSD)
	{
		fileSD.close();
	}

	int writeData(SD_File fileSD, sens_data::GpsData gpsData, sens_data::IMUData imuData, sens_data::BarometerData barData, sens_data::BatteryData batData)
    {
		if(!fileSD) 
		{
			Serial.println("Unbale to write to SD card");
			return 0;
		}

        //Flash timing
        float time = flash::getTimeElapsed();

        //GPS
        float lat = gpsData.lat; //1.1
        float lng = gpsData.lng; //1.2
        float alt = gpsData.alt; //1.3
        float sats = gpsData.sats; //1.4

        //Bar
        float pressure = barData.pressure; //2.1
        float altitude = barData.altitude; //2.2
        float vert_velocity = barData.f_velocity; //2.3
        float temperature = barData.temperature; //2.4
       
        //Bat
        float bat1 = batData.bat1; //3.1
        float bat2 = batData.bat2; //3.2

        //Mag
    	float mag_x = imuData.mag_x; //4.1
        float mag_y = imuData.mag_y; //4.2
        float mag_z = imuData.mag_z; //4.3
        float acc_x = imuData.acc_x; //4.4
        float acc_y = imuData.acc_y; //4.5
        float acc_z = imuData.acc_z; //4.6

		fileSD.println(String(time, 2)+","+String(lat, 4)+","+String(lng, 4)+","+String(alt, 2)+","+String(sats)+","+String(pressure, 2)+","+String(altitude, 2)+","+String(vert_velocity, 2)+","+String(temperature, 1)+","+String(bat1, 2)+","+String(bat2, 2)+","+String(mag_x, 2)+","+String(mag_y, 2)+","+String(mag_z, 2)+","+String(acc_x, 2)+","+String(acc_y, 2)+","+String(acc_z, 2));
		return 1;
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