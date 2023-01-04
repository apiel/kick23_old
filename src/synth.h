#ifndef SYNTH_H_
#define SYNTH_H_

#include <stdint.h>

#include "def.h"
#include "wavetable_sine.h"
#include "wavetable_square.h"

#define WT_FRAME_SAMPLE_COUNT 2048

float frequency = 600.0f;

// 90ms kick duration
// uint sampleCountDuration = 90 * SAMPLE_PER_MS;
unsigned int sampleCountDuration = 300 * SAMPLE_PER_MS;
unsigned int sampleCount = -1; // set it to max uint value so it will not trigger the kick at the beginning

#define ENVELOP_STEPS 5

// REMOVE clicking from beginning
// float envelopAmp[ENVELOP_STEPS][2] = { { 0.0f, 0.0f }, { 1.0f, 0.01f }, { 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 1.0f } };
float envelopAmp[ENVELOP_STEPS][2] = { { 0.0f, 0.0f }, { 1.0f, 0.01f }, { 0.3f, 0.4f }, { 0.0f, 1.0f }, { 0.0f, 1.0f } };
float envelopFreq[ENVELOP_STEPS][2] = { { 1.0f, 0.0f }, { 0.26f, 0.03f }, { 0.24f, 0.35f }, { 0.22f, 0.4f }, { 0.0f, 1.0f } };

unsigned int envelopAmpIndex = 0;
unsigned int envelopFreqIndex = 0;

float IRAM_ATTR envelop(float (*envelop)[2], unsigned int* envelopIndex)
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

float IRAM_ATTR getSample()
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
        sampleCount++;
        // return ((WT_Sine[(uint16_t)sampleIndex] / 32768.0) * 255.0 + 128.0) * envAmp;
        return (WT_Sine[(uint16_t)sampleIndex] / 32768.0) * envAmp;
    }
    return 0;
}

void triggerSound()
{
    sampleCount = 0;
    envelopAmpIndex = 0;
    envelopFreqIndex = 0;
}

void buttonPressed()
{
    // Here the button could do other function
    triggerSound();
}

void buttonReleased()
{
    // Here the button could do other function
}

int counterRT = 0;
void rotaryChanged(int8_t direction)
{
    counterRT += direction;
    APP_LOG("rotaryChanged: %d\n", counterRT);
}

void rotaryPressed()
{
    APP_LOG("rotaryPressed\n");
}

#define POT_COUNT 3

uint8_t potValue[POT_COUNT] = { 0, 0, 0 };
void updatePot(uint8_t potIndex, uint8_t value)
{
    if (potIndex < POT_COUNT && value != potValue[potIndex]) {
        APP_LOG("Pot %d value changed: %d\n", potIndex, potValue[potIndex]);
        potValue[potIndex] = value;
    }
}

#endif