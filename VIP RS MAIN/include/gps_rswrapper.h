//#pragma once
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

//TODO izchekot kas notiek ar gps - kaa saglabat vecas

namespace gps {

    #define RXPIN 4 //P4 on lopy is set to RX pin (important to remember about crossover connection)
    #define TXPIN 15  //P3
    SoftwareSerial gpsSerial;
    SoftwareSerialConfig ssc = SWSERIAL_8N1; // 8bits-no_parity-1_stop_bit  https://github.com/plerup/espsoftwareserial/
    TinyGPSPlus gps;

    boolean hasData = false;

    void setup(uint gpsRate = 9600)
    {
        gpsSerial.begin(gpsRate, ssc, RXPIN, TXPIN);
        Serial.println("Init GPS: " + String(gpsRate));
    }

    void readGps()
    {
        hasData = false;

        while (gpsSerial.available())
        {
            gps.encode(gpsSerial.read());
            hasData = true;
        }
    }

    double lastLatitude() {
        return gps.location.lat();  //TODO ar 6 skaitljiem?
    }

    double lastLongitude() {
        return gps.location.lng();
    }

    double lastAltitude() {
        return gps.altitude.meters();
    }

    //*NEW

    int getSatellites()
    {
        return gps.satellites.value();
    }

    bool gpsValid()
    {
        return gps.location.isValid();
    }

    double getDistance(double latRocket, double longRocket)
    {
        return gps.distanceBetween(gps.location.lat(), gps.location.lng(), latRocket, longRocket);
    }

    double getCourseTo(double latRocket, double longRocket)
    {
        return gps.courseTo(gps.location.lat(), gps.location.lng(), latRocket, longRocket);
    }
}