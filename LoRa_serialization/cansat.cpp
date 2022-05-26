/*
  Home
*/
#include <MPU9250_asukiaaa.h>
#include <Adafruit_BMP280.h>
#include <SPI.h>
#include <SD.h>

File myFile;
Adafruit_BMP280 bme; // I2C
MPU9250_asukiaaa mySensor;
float aX, aY, aZ, aSqrt, gX, gY, gZ, mDirection, mX, mY, mZ;

void setup() {
  Serial.begin(9600);
  while (!Serial);

#ifdef _ESP32_HAL_I2C_H_ // For ESP32
  Wire.begin(SDA_PIN, SCL_PIN);
  mySensor.setWire(&Wire);
#else
  Wire.begin();
  mySensor.setWire(&Wire);
#endif

  bme.begin();
  mySensor.beginAccel();
  mySensor.beginGyro();
  mySensor.beginMag();

  Serial.print("Initializing SD card...");
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
while (1);
}
Serial.println("initialization done.");

myFile = SD.open("test.txt", FILE_WRITE);
}

void loop() {

if (myFile) {

  if (mySensor.accelUpdate() == 0) {
    aX = mySensor.accelX();
    aY = mySensor.accelY();
    aZ = mySensor.accelZ();
    aSqrt = mySensor.accelSqrt();
    myFile.println("accelX: " + String(aX));
    myFile.println("\taccelY: " + String(aY));
    myFile.println("\taccelZ: " + String(aZ));
    myFile.println("\taccelSqrt: " + String(aSqrt));
  }

  if (mySensor.gyroUpdate() == 0) {
    gX = mySensor.gyroX();
    gY = mySensor.gyroY();
    gZ = mySensor.gyroZ();
    myFile.println("\tgyroX: " + String(gX));
    myFile.println("\tgyroY: " + String(gY));
    myFile.println("\tgyroZ: " + String(gZ));
  }

  if (mySensor.magUpdate() == 0) {
    mX = mySensor.magX();
    mY = mySensor.magY();
    mZ = mySensor.magZ();
    mDirection = mySensor.magHorizDirection();
    myFile.println("\tmagX: " + String(mX));
    myFile.println("\tmaxY: " + String(mY));
    myFile.println("\tmagZ: " + String(mZ));
    myFile.println("\thorizontalDirection: " + String(mDirection));
  }

  myFile.println("\tTemperature(*C): ");
  myFile.println(bme.readTemperature());

  myFile.println("\tPressure(Inches(Hg)): ");
  myFile.println(bme.readPressure() / 3377);


  myFile.println("\tApproxAltitude(m): ");
  myFile.println(bme.readAltitude(1013.25)); // this should be adjusted to your local forcase

  // close the file:

} 
else {
// if the file didn't open, print an error:
Serial.println("error opening test.txt");
}

myFile.close();
Serial.println("done.");
}