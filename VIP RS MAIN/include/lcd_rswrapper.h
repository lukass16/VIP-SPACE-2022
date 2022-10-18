#pragma once
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <string>
#include <sstream>

// https://github.com/johnrickman/LiquidCrystal_I2C
// https://docs.google.com/document/d/1Fbtc8zCtQmf6O5GGj64akNSewvqAAdK0V5oWDq2YLxc/edit

namespace lcd
{

    LiquidCrystal_I2C lcd(0x27, 20, 4);

    void setup()
    {
        lcd.init();
        lcd.backlight();
        lcd.setCursor(1, 0); // Pirmais skaitlis ir kolonna, otrais skatilis ir rinda
        lcd.print("Press <> to begin");
    }

    void writeLat(double lat)
    {
        lcd.setCursor(0, 0);
        lcd.print("Lat:");
        lcd.setCursor(4, 0);
        if (lat != 0)
        {
            lcd.print(lat, 4);
        }
        else
        {
            lcd.print("NL");
        }
    }

    void writeLon(double lon)
    {
        lcd.setCursor(0, 1);
        lcd.print("Lon:");
        lcd.setCursor(4, 1);
        if (lon != 0)
        {
            lcd.print(lon, 4);
        }
        else
        {
            lcd.print("NL");
        }
    }

    void writeDistance(double dist)
    {
        lcd.setCursor(0, 2);
        lcd.print("Dist:");
        lcd.setCursor(5, 2);
        lcd.print(dist, 0);
    }

    void writeHeading(double course)
    {
        lcd.setCursor(0, 3);
        lcd.print("HDNG:");
        lcd.setCursor(5, 3);
        lcd.print(course, 0);
    }

    void writeAltitude(double bar_alt)
    {
        lcd.setCursor(12, 2);
        lcd.print("Alt:");
        lcd.setCursor(16, 2);
        lcd.print(bar_alt, 0); // Using barometric altitude rather than GPS
    }

    void writeState(int r_state)
    {
        lcd.setCursor(12, 1);
        lcd.print("S:");
        lcd.setCursor(14, 1);
        lcd.print(r_state);
    }

    void writeSpeed(float spd)
    {
        lcd.setCursor(12, 3);
        lcd.print("SPD:");
        lcd.setCursor(16, 3);
        lcd.print(spd, 0);
    }

    void writeCounter(int counter)
    {
        lcd.setCursor(12, 0);
        lcd.print("C:");
        lcd.setCursor(14, 0);
        lcd.print(counter);
    }

    void showGPSinfo(int sats_rs, int sats_fc, int gpsValid, bool canWriteToFlash)
    {
        lcd.clear();
        if (sats_rs <= 3)
        {
            lcd.setCursor(0, 0);
            lcd.print("Finding GPS sats");
        }
        else
        {
            lcd.setCursor(0, 0);
            lcd.print("3D Lock Established");
        }
        // print RS visible sats
        lcd.setCursor(0, 1);
        lcd.print("Sats visible RS: ");
        lcd.print(sats_rs);

        // print FC visible sats
        lcd.setCursor(0, 2);
        lcd.print("Sats visible FC: ");
        lcd.print(sats_fc);

        //print if local GPS is getting valid readings
        lcd.setCursor(0, 3);
        lcd.print("GPS is valid: ");
        lcd.print(gpsValid);

        if (canWriteToFlash)
        {
            lcd.setCursor(19, 3);
            lcd.print("W");
        }
    }

    void showLoRaInfo(int lastCounter, int lostPackets, double successRate, float receivedRSSI, float receivedSNR, int corruptedPackets)
    {
        int droppedPackets = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Received:");
        lcd.print(lastCounter - lostPackets);
        lcd.print("/");
        lcd.print(lastCounter);

        lcd.setCursor(0, 1);
        lcd.print("CRPT:");
        lcd.print(corruptedPackets);
        lcd.print("    DROP:");
        droppedPackets = lostPackets - corruptedPackets;
        lcd.print(droppedPackets);

        lcd.setCursor(0, 2);
        lcd.print("SNR:");
        lcd.print(receivedSNR, 0);
        lcd.print("dB   OK:");
        lcd.print(successRate, 0);
        lcd.print("%");

        lcd.setCursor(0, 3);
        lcd.print("RSSI:");
        lcd.print(receivedRSSI, 0);
        lcd.print("dBm");
    }

    void showIncomingData(double lat, double lon, double dist, double course, double alt, float spd, int r_state, int counter)
    {
        lcd.clear();
        writeLat(lat);
        writeLon(lon);
        writeDistance(dist);
        writeHeading(course);
        writeAltitude(alt);
        writeSpeed(spd);
        writeState(r_state);
        writeCounter(counter);
    }

}
