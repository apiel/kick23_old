#include <Arduino.h>
#include <U8g2lib.h>

#ifndef APP_LOG
#define APP_LOG Serial.printf
#endif

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
#define AUDIO_PIN 3 // is adc pin might want to change
#define BUTTON_PIN 21
#define POT1_PIN 1

#define ROTARY_CLK_PIN 6
#define ROTARY_DT_PIN 5
#define ROTARY_SW_PIN 4 // is adc pin might want to change

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

////// ROTARY ENCODER

int currentStateCLK;
int lastStateCLK;
unsigned long rotaryLastPush = 0;

void handleRotaryEncoder()
{
    currentStateCLK = digitalRead(ROTARY_CLK_PIN);
    // React to only HIGH state change to avoid double count
    if (currentStateCLK != lastStateCLK && currentStateCLK == HIGH) {
        rotaryChanged(digitalRead(ROTARY_DT_PIN) == currentStateCLK ? -1 : 1);
    }
    lastStateCLK = currentStateCLK;

    int btnState = digitalRead(ROTARY_SW_PIN);
    if (btnState == LOW) {
        if (millis() - rotaryLastPush > 50) {
            // Serial.println("Rotary Button pressed!");
            rotaryPressed();
        }
        rotaryLastPush = millis();
    }
}

////// ROTARY ENCODER END

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

    // Set encoder pins as inputs
    pinMode(ROTARY_CLK_PIN, INPUT);
    pinMode(ROTARY_DT_PIN, INPUT);
    pinMode(ROTARY_SW_PIN, INPUT_PULLUP);
    lastStateCLK = digitalRead(ROTARY_CLK_PIN);

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

uint8_t counter = 0;
void loop()
{
    if (counter < 3) {
        Serial.println("Trigger sound");
        triggerSound();
        delay(1000);
        counter++;
    }

    int currentState = digitalRead(BUTTON_PIN);
    if (currentState != lastState) {
        if (currentState == LOW) {
            Serial.println("Button pressed");
            buttonPressed();
        } else {
            Serial.println("Button released");
            buttonReleased();
        }
    }
    lastState = currentState;

    // FIXME dont do * 100 and round here...
    // 1 / 4095 * 100 = 0.0244140625
    updatePot(0, roundf(analogRead(POT1_PIN) * 0.0244140625));

    handleRotaryEncoder();
}
