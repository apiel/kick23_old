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
#define MaxWaveTypes 4 // types of wave (signal)

// static byte waveTable[MaxWaveTypes][WT_FRAME_SAMPLE_COUNT] = {
//     // Sin wave
//     {
//         0x80, 0x83, 0x87, 0x8A, 0x8E, 0x91, 0x95, 0x98, 0x9B, 0x9E, 0xA2, 0xA5, 0xA7, 0xAA, 0xAD, 0xAF,
//         0xB2, 0xB4, 0xB6, 0xB8, 0xB9, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xBF, 0xBF, 0xC0, 0xBF, 0xBF, 0xBF,
//         0xBE, 0xBD, 0xBC, 0xBB, 0xB9, 0xB8, 0xB6, 0xB4, 0xB2, 0xAF, 0xAD, 0xAA, 0xA7, 0xA5, 0xA2, 0x9E,
//         0x9B, 0x98, 0x95, 0x91, 0x8E, 0x8A, 0x87, 0x83, 0x80, 0x7C, 0x78, 0x75, 0x71, 0x6E, 0x6A, 0x67,
//         0x64, 0x61, 0x5D, 0x5A, 0x58, 0x55, 0x52, 0x50, 0x4D, 0x4B, 0x49, 0x47, 0x46, 0x44, 0x43, 0x42,
//         0x41, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x41, 0x42, 0x43, 0x44, 0x46, 0x47, 0x49, 0x4B,
//         0x4D, 0x50, 0x52, 0x55, 0x58, 0x5A, 0x5D, 0x61, 0x64, 0x67, 0x6A, 0x6E, 0x71, 0x75, 0x78, 0x7C },
//     // Triangular wave table
//     {
//         0x80, 0x84, 0x89, 0x8D, 0x92, 0x96, 0x9B, 0x9F, 0xA4, 0xA8, 0xAD, 0xB2, 0xB6, 0xBB, 0xBF, 0xC4,
//         0xC8, 0xCD, 0xD1, 0xD6, 0xDB, 0xDF, 0xE4, 0xE8, 0xED, 0xF1, 0xF6, 0xFA, 0xFF, 0xFA, 0xF6, 0xF1,
//         0xED, 0xE8, 0xE4, 0xDF, 0xDB, 0xD6, 0xD1, 0xCD, 0xC8, 0xC4, 0xBF, 0xBB, 0xB6, 0xB2, 0xAD, 0xA8,
//         0xA4, 0x9F, 0x9B, 0x96, 0x92, 0x8D, 0x89, 0x84, 0x7F, 0x7B, 0x76, 0x72, 0x6D, 0x69, 0x64, 0x60,
//         0x5B, 0x57, 0x52, 0x4D, 0x49, 0x44, 0x40, 0x3B, 0x37, 0x32, 0x2E, 0x29, 0x24, 0x20, 0x1B, 0x17,
//         0x12, 0x0E, 0x09, 0x05, 0x00, 0x05, 0x09, 0x0E, 0x12, 0x17, 0x1B, 0x20, 0x24, 0x29, 0x2E, 0x32,
//         0x37, 0x3B, 0x40, 0x44, 0x49, 0x4D, 0x52, 0x57, 0x5B, 0x60, 0x64, 0x69, 0x6D, 0x72, 0x76, 0x7B },

//     // Sawtooth wave table
//     {
//         0x00, 0x02, 0x04, 0x06, 0x09, 0x0B, 0x0D, 0x10, 0x12, 0x14, 0x16, 0x19, 0x1B, 0x1D, 0x20, 0x22,
//         0x24, 0x27, 0x29, 0x2B, 0x2D, 0x30, 0x32, 0x34, 0x37, 0x39, 0x3B, 0x3E, 0x40, 0x42, 0x44, 0x47,
//         0x49, 0x4B, 0x4E, 0x50, 0x52, 0x54, 0x57, 0x59, 0x5B, 0x5E, 0x60, 0x62, 0x65, 0x67, 0x69, 0x6B,
//         0x6E, 0x70, 0x72, 0x75, 0x77, 0x79, 0x7C, 0x7E, 0x80, 0x82, 0x85, 0x87, 0x89, 0x8C, 0x8E, 0x90,
//         0x93, 0x95, 0x97, 0x99, 0x9C, 0x9E, 0xA0, 0xA3, 0xA5, 0xA7, 0xA9, 0xAC, 0xAE, 0xB0, 0xB3, 0xB5,
//         0xB7, 0xBA, 0xBC, 0xBE, 0xC0, 0xC3, 0xC5, 0xC7, 0xCA, 0xCC, 0xCE, 0xD1, 0xD3, 0xD5, 0xD7, 0xDA,
//         0xDC, 0xDE, 0xE1, 0xE3, 0xE5, 0xE8, 0xEA, 0xEC, 0xEE, 0xF1, 0xF3, 0xF5, 0xF8, 0xFA, 0xFC, 0xFE },
//     // Square wave table
//     {
//         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
// };

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

#define ENVELOP_STEPS 3

// float envelopFreq[ENVELOP_STEPS][2] = { { 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 1.0f } };
// float envelopAmp[ENVELOP_STEPS][2] = { { 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 1.0f } };

// float envelopFreq[ENVELOP_STEPS][2] = { { 0.3f, 0.05f }, { 0.1f, 0.3f }, { 0.05f, 0.5f } };
float envelopAmp[ENVELOP_STEPS][2] = { { 0.6f, 0.1f }, { 0.9f, 0.15f }, { 0.7f, 0.6f } };
float envelopFreq[ENVELOP_STEPS][2] = { { 0.26f, 0.03f }, { 0.25f, 0.31f }, { 0.24f, 0.32f } };

struct EnvelopStatus {
    float value;
    float start[2];
    float target[2];
    int stepIndex;
    float stepAccumulator;
};

EnvelopStatus envelopAmpStatus;
EnvelopStatus envelopFreqStatus;

void IRAM_ATTR setEnvelopStatus(EnvelopStatus* status, float (*envelop)[2], int index)
{
    status->start[0] = 1.0f; // We assume the envelop starts at 1.0f
    status->start[1] = 0.0f;

    status->target[0] = 0.0f; // We assume the envelop ends at 0.0f
    status->target[1] = 1.0f;

    if (index > 0) {
        status->start[0] = envelop[index - 1][0];
        status->target[1] = envelop[index - 1][1];
    }
    if (index < ENVELOP_STEPS) {
        status->target[0] = envelop[index][0];
        status->target[1] = envelop[index][1];
    }

    float count = sampleCountDuration * (status->target[1] - status->start[1]);
    if (count) {
        status->stepAccumulator = (status->start[0] - status->target[0]) / count;
    } else {
        status->stepAccumulator = status->start[0] - status->target[0];
    }

    status->stepIndex = index;
}

void resetEnvelopStatus(EnvelopStatus* status, float (*envelop)[2])
{
    status->value = 1.0f;
    setEnvelopStatus(status, envelopFreq, 0);
}

float IRAM_ATTR envelop(EnvelopStatus* status, float (*envelop)[2])
{
    if (status->stepIndex > ENVELOP_STEPS) {
        return 0.0f;
    }

    status->value += status->stepAccumulator;

    if (sampleCount >= sampleCountDuration * status->target[1]) {
        status->stepIndex++;
        if (status->stepIndex < ENVELOP_STEPS + 1) {
            setEnvelopStatus(status, envelop, status->stepIndex);
        }
    }

    return status->value;
}

float midVal[3] = { 1.0f, 0.26f, 0.0f };
float midTime[3] = { 0.0f, 0.03f, 1.0f };

int_fast64_t midIndex = 0;

float IRAM_ATTR envelop2()
{
    float time = (float)sampleCount / (float)sampleCountDuration;
    if (time >= midTime[midIndex+1]) {
       midIndex++;
    }
    float timeOffset = midTime[midIndex+1] - midTime[midIndex];
    float timeRatio = (time - midTime[midIndex]) / timeOffset;
    return (midVal[midIndex+1] - midVal[midIndex]) * timeRatio + midVal[midIndex];
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
        // float envAmp = envelop(&envelopAmpStatus, envelopAmp);
        // float envFreq = envelop(&envelopFreqStatus, envelopFreq);
        // float envAmp = envelop();
        // float envFreq = 1.0f - envelop();
        float envFreq = envelop2();
        float envAmp = 1.0f;
        // float envFreq = 0.0f;

        // sampleStep = WT_FRAME_SAMPLE_COUNT * (frequency + (envFreq * freqModulationRange)) / SAMPLE_RATE;
        sampleStep = WT_FRAME_SAMPLE_COUNT * (frequency * envFreq) / SAMPLE_RATE;

        sampleIndex += sampleStep;
        while (sampleIndex >= WT_FRAME_SAMPLE_COUNT) {
            sampleIndex -= WT_FRAME_SAMPLE_COUNT;
        }
        // ledcWrite(ledChannel, waveTable[3][(uint16_t)sampleIndex] * envAmp);
        // ledcWrite(ledChannel, WT_Sine[(uint16_t)sampleIndex] * envAmp);
        ledcWrite(ledChannel, (WT_Sine[(uint16_t)sampleIndex] / 32768.0) * envAmp * 255.0 + 128.0);
        sampleCount++;
    } else {
        ledcWrite(ledChannel, 0);
    }
}

void triggerKick()
{
    midIndex = 0;
    sampleCount = 0;
    resetEnvelopStatus(&envelopAmpStatus, envelopAmp);
    resetEnvelopStatus(&envelopFreqStatus, envelopFreq);
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
