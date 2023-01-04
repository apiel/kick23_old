#include <Arduino.h>
#include <U8g2lib.h>

#include "def.h"
#include "synth.h"
#include "wavetable_square.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define SDA_PIN 5
#define SCL_PIN 6
#define AUDIO_PIN 3
#define BUTTON_PIN 21

#define PWM_CHANNEL 0

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

// Use timer to generate sound frequency
void IRAM_ATTR onTimer()
{
    // ledcWrite(PWM_CHANNEL, getSample() * 512.0);
    ledcWrite(PWM_CHANNEL, getSample() * 800.0);
}

void setup()
{
    Serial.begin(115200);
    pinMode(BUTTON_PIN, INPUT_PULLUP); // config GIOP21 as input pin and enable the internal pull-up resistor

    // configure LED PWM functionalitites
    ledcSetup(PWM_CHANNEL, SAMPLE_RATE, 8);

    // attach the channel to the GPIO to be controlled
    ledcAttachPin(AUDIO_PIN, PWM_CHANNEL);

    // on ESP32 timer run at 80MHz = 80,000,000 ticks per second
    // since we only need 44.1kHz, we can use a divider of 1814
    hw_timer_t* timer = timerBegin(0, 1814, true);
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, 1, true); // Trigger every 1 tick
    timerAlarmEnable(timer);
}

// Variables will change:
int lastState = HIGH; // the previous state from the input pin
int currentState; // the current reading from the input pin

uint8_t counter = 0;
void loop()
{
    if (counter < 3) {
        Serial.println("Trigger sound");
        triggerSound();
        delay(1000);
        counter++;
    }

    currentState = digitalRead(BUTTON_PIN);
    if (lastState == LOW && currentState == HIGH) {
        Serial.println("The state changed from LOW to HIGH");
        triggerSound();
    }
    lastState = currentState;
}
