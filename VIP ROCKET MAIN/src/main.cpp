#include "states/preperation_state.cpp"

Context *mode = new Context(new PreperationState);

void setup() {
    Serial.begin(115200);

    mode->Start();
}
 
void loop() {}


/*
The following list contains all of the most important code parameters that can be tuned and where to find them

Detection parameters:
1) Launch detection acceleration threshold - include/imu_wrapper_MPU9250.h/launchDetected() -> threshold
2) Launch detection times of threshold detection - include/imu_wrapper_MPU9250.h/launchDetected() -> times
3) Apogee timer duration - include/arming.h/startApogeeTimer() -> timerLength
4) Main ejection threshold altitude - include/barometer_wrapper_MS5607.h/mainAltitudeDetected() -> threshold
5) Main ejection times of threshold altitude detection - include/barometer_wrapper_MS5607.h/mainAltitudeDetected() -> times
6) Touchdown timer duration - include/arming.h/startTouchdownTimer() -> timerLength

Memory writing, operational parameters:
7) Flash writing time, the time after main ejection during which data is written to flash - include/states/descent_state.cpp/ -> flash_write_time (NOTE: has to be less than touchdown timer duration)
8) Flash Drogue state close open interval, amount of loops after which the flash is closed and opened - include/states/drogue_state.cpp/ -> interval
9) Flash Main state close open interval, amount of loops after which the flash is closed and opened - include/states/main_state.cpp/ -> interval
10) Flash Descent state close open interval, amount of loops after which the flash is closed and opened - include/states/descent_state.cpp/ -> interval
11) Arming loop delay - include/states/preperation_state.cpp/delay()
12) Launch loop delay - include/states/drogue_state.cpp/delay()#1
13) Apogee loop delay - include/states/drogue_state.cpp/delay()#2
14) Main loop delay - include/states/main_state.cpp/delay()
15) Pre-touchdown loop delay - include/states/descent_state.cpp/delay()#1
16) Post-touchdown loop delay - include/states/descent_state.cpp/delay()#2

Kalman filter parameters:
17) Q, process noise uncertainty - include/kalman.h -> Q
18) P, estimate uncertainty - include/kalman.h -> P
19) R, measurement uncertainty - include/kalman.h -> R
*/
