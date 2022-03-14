// https://randomnerdtutorials.com/esp32-dual-core-arduino-ide/

// Important - Do not use sleep() and delay() functions inbetween mutex locking scope!


#pragma once
#include <Arduino.h>

// Meaning 'second thread'
namespace s_thread {
    TaskHandle_t taskHandle;

    // f - function that will be executed in a seperate thread
    // pvParameters - c style arguments to function
    template <typename F>
    inline void setup(F f, void *pvParameters = NULL) {
        xTaskCreatePinnedToCore(
            f, /* Function to implement the task */
            "Task1", /* Name of the task */
            10000,  /* Stack size in words */
            NULL,  /* Task input parameter */
            0,  /* Priority of the task */
            &taskHandle,  /* Task handle. */
            0 /* Core where the task should run */
        ); 
    }

};
