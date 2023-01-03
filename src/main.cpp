#include <Arduino.h>
#include <U8g2lib.h>

#include "def.h"
#include "wavetable_sine.h"
#include "wavetable_square.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
#define SDA_PIN 5
#define SCL_PIN 6

// #define WT_FRAME_SAMPLE_COUNT 112 //  number of dample of signal
#define WT_FRAME_SAMPLE_COUNT 2048

// U8G2_SSD1306_72X40_ER_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);   // EastRising 0.42" OLED

// void setup(void) {
//   Wire.begin(SDA_PIN, SCL_PIN);
//   u8g2.begin();
// }

// void loop(void) {
//   u8g2.clearBuffer();					// clear the internal memory
//   u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
//   u8g2.drawStr(0,10,"Hello World!");	// write something to the internal memory
//   u8g2.sendBuffer();					// transfer internal memory to the display
//   delay(1000);
// }

// the number of the LED pin
const int ledPin = 3;
const int ledChannel = 0;

float frequency = 600.0f;

// 90ms kick duration
// uint sampleCountDuration = 90 * SAMPLE_PER_MS;
uint sampleCountDuration = 300 * SAMPLE_PER_MS;
uint sampleCount = -1; // set it to max uint value so it will not trigger the kick at the beginning

#define ENVELOP_STEPS 5

// REMOVE clicking from beginning
// float envelopAmp[ENVELOP_STEPS][2] = { { 0.0f, 0.0f }, { 1.0f, 0.01f }, { 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 1.0f } };
float envelopAmp[ENVELOP_STEPS][2] = { { 0.0f, 0.0f }, { 1.0f, 0.01f }, { 0.3f, 0.4f }, { 0.0f, 1.0f }, { 0.0f, 1.0f } };
float envelopFreq[ENVELOP_STEPS][2] = { { 1.0f, 0.0f }, { 0.26f, 0.03f }, { 0.24f, 0.35f }, { 0.22f, 0.4f }, { 0.0f, 1.0f } };

uint envelopAmpIndex = 0;
uint envelopFreqIndex = 0;

float IRAM_ATTR envelop(float (*envelop)[2], uint* envelopIndex)
{
    if (envelopFreqIndex > ENVELOP_STEPS - 1) {
        return 0.0f;
    }

    float time = (float)sampleCount / (float)sampleCountDuration;
    if (time >= envelop[*envelopIndex + 1][1]) {
        (*envelopIndex)++;
    }
    float timeOffset = envelop[*envelopIndex + 1][1] - envelop[*envelopIndex][1];
    float timeRatio = (time - envelop[*envelopIndex][1]) / timeOffset;
    return (envelop[*envelopIndex + 1][0] - envelop[*envelopIndex][0]) * timeRatio + envelop[*envelopIndex][0];
}

float IRAM_ATTR envelop()
{
    return (float)sampleCount / (float)sampleCountDuration;
}

float sampleIndex = 0.0f;
float sampleStep = 1.0f;

void IRAM_ATTR onTimer()
{
    if (sampleCount < sampleCountDuration) {
        // float envAmp = envelop();
        // float envFreq = 1.0f - envelop();
        // float envFreq = envelop2();
        float envFreq = envelop(envelopFreq, &envelopFreqIndex);
        float envAmp = envelop(envelopAmp, &envelopAmpIndex);
        // float envAmp = 1.0f;
        // float envFreq = 0.0f;

        // sampleStep = WT_FRAME_SAMPLE_COUNT * (frequency + (envFreq * freqModulationRange)) / SAMPLE_RATE;
        sampleStep = WT_FRAME_SAMPLE_COUNT * (frequency * envFreq) / SAMPLE_RATE;

        sampleIndex += sampleStep;
        while (sampleIndex >= WT_FRAME_SAMPLE_COUNT) {
            sampleIndex -= WT_FRAME_SAMPLE_COUNT;
        }
        ledcWrite(ledChannel, ((WT_Sine[(uint16_t)sampleIndex] / 32768.0) * 255.0 + 128.0) * envAmp);
        sampleCount++;
    } else {
        ledcWrite(ledChannel, 0);
    }
}

void triggerKick()
{
    sampleCount = 0;
    envelopAmpIndex = 0;
    envelopFreqIndex = 0;
}

void setup()
{
    // configure LED PWM functionalitites
    ledcSetup(ledChannel, SAMPLE_RATE, 8);

    // attach the channel to the GPIO to be controlled
    ledcAttachPin(ledPin, ledChannel);

    // on ESP32 timer run at 80MHz = 80,000,000 ticks per second
    // since we only need 44.1kHz, we can use a divider of 1814
    hw_timer_t* timer = timerBegin(0, 1814, true);
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, 1, true); // Trigger every 1 tick
    timerAlarmEnable(timer);
}

uint8_t counter = 0;
void loop()
{
    if (counter < 4) {
        triggerKick();
        delay(2000);
        counter++;
    }
}
