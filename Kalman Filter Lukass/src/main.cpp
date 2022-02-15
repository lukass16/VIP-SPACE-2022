#include <Arduino.h>
#include <Kalman.h>

float p_baro = 0;
float v_baro = 5; //*testing with constant velocity model (a=0)

void setup() {
  Serial.begin(115200);
  Serial.println("Kalman Filter tests");
  kalman::predict();
}

void loop() {
  kalman::predict();
  delay(500);
  kalman::printKalmanState();
  kalman::printPostionUncertainty();
  kalman::updateBaro(p_baro); //this doesn't necessarily have to be done right before prediction (extrapolation) but can be just done every time a new reading is available

  //change p_baro for test
  p_baro = 0.5 * v_baro;
}