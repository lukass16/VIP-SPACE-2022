#include "states/standby_state.cpp"

Context *mode = new Context(new StandByState);

void setup() {
    Serial.begin(115200);

    mode->Start();
}
 
void loop() {}