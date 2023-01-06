#include <Arduino.h>

#include "dac_samd51.h"

#ifndef APP_LOG
#define APP_LOG Serial.printf
#endif

#include "synth.h"

void audioCallback(const uint32_t* end, uint32_t* dest)
{
    do {
        uint32_t sample = getSample() * 0x7FFF;
        // uint32_t sample = getSample() * 16383;
        // uint32_t sample = getSample() * 2147483647;
        *dest++ = sample;
        // *dest++ = sample;
    } while (dest < end);
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Kick23");

    initDac(audioCallback);
}

uint8_t counter = 0;
void loop()
{
    // if (counter < 3) {
    if (counter < 10) {
        Serial.println("Trigger sound");
        triggerSound();
        delay(1000);
        counter++;
    }
}
