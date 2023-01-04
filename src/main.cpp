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

void IRAM_ATTR onTimer()
{
    // ledcWrite(ledChannel, getSample() * 512.0);
    ledcWrite(ledChannel, getSample() * 800.0);
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
        triggerSound();
        delay(2000);
        counter++;
    }
}
