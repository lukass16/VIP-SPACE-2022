#pragma once
#include <Arduino.h>

struct Mtx {
    SemaphoreHandle_t xMutex;
    
    inline void setup(){
        xMutex = xSemaphoreCreateMutex();
    }

    // max_lock_time - how long to lock the mutex? Defaults to infinite
    inline void lock(TickType_t max_lock_time = portMAX_DELAY){
        if(!xMutex)
            Serial.println("mutex hasn't been set! Mutex == NULL");

        xSemaphoreTake(xMutex, portMAX_DELAY); 
    }

    inline void unlock(){
        xSemaphoreGive(xMutex);
    }
};