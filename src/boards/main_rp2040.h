#include <Arduino.h>
#include <I2S.h>
#include <U8g2lib.h>

#include "RPi_Pico_TimerInterrupt.h"

#ifndef APP_LOG
#define APP_LOG Serial.printf
#endif

#include "def.h"
#include "synth.h"
#include "wavetable_square.h"

#include <Wire.h>

U8G2_SSD1306_72X40_ER_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE); // EastRising 0.42" OLED

#define SDA_PIN 22
#define SCL_PIN 23

// Create the I2S port using a PIO state machine
I2S i2s(OUTPUT);

// GPIO pin numbers
#define pBCLK 24
#define pWS (pBCLK + 1)
#define pDOUT 26

///// Timer

RPI_PICO_Timer ITimer1(1);

#define TIMER1_INTERVAL_MS 1000

bool TimerHandler1(struct repeating_timer* t)
{

    Serial.println("TimerHandler1");

    return true;
}

///// Timer END

const int sampleRate = 16000; // minimum for UDA1334A

void setup()
{
    Wire.setSDA(SDA_PIN);
    Wire.setSCL(SCL_PIN);
    Wire.begin();
    u8g2.begin();
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_7x14_tf);
    u8g2.drawStr(0, 10, "yo2");
    u8g2.sendBuffer();

    Serial.begin(115200);

    i2s.setBCLK(pBCLK);
    i2s.setDATA(pDOUT);
    i2s.setBitsPerSample(16);

    // start I2S at the sample rate with 16-bits per sample
    i2s.begin(sampleRate);

    ITimer1.attachInterruptInterval(TIMER1_INTERVAL_MS * 1000, TimerHandler1);
}

const int freq = 440; // frequency of square wave in Hz
const int amplitude = 500; // amplitude of square wave

const int halfWavelength = (sampleRate / freq); // half wavelength of square wave

int16_t sample = amplitude; // current sample value
int count = 0;

void loop()
{
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_7x14_tf);
    u8g2.drawStr(0, 10, "yo2");
    u8g2.sendBuffer();
    // delay(1000);

    if (count % halfWavelength == 0) {
        // invert the sample every half wavelength count multiple to generate square wave
        sample = -1 * sample;
    }

    // write the same sample twice, once for left and once for the right channel
    i2s.write(sample);
    i2s.write(sample);

    // increment the counter for the next sample
    count++;
}
