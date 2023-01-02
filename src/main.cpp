#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
#define SDA_PIN 5
#define SCL_PIN 6

#define Num_Samples  112   //  number of dample of signal
#define MaxWaveTypes 4   // types of wave (signal)

static byte WaveFormTable[MaxWaveTypes][Num_Samples] = {
  // Sin wave
  {
    0x80, 0x83, 0x87, 0x8A, 0x8E, 0x91, 0x95, 0x98, 0x9B, 0x9E, 0xA2, 0xA5, 0xA7, 0xAA, 0xAD, 0xAF,
    0xB2, 0xB4, 0xB6, 0xB8, 0xB9, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xBF, 0xBF, 0xC0, 0xBF, 0xBF, 0xBF,
    0xBE, 0xBD, 0xBC, 0xBB, 0xB9, 0xB8, 0xB6, 0xB4, 0xB2, 0xAF, 0xAD, 0xAA, 0xA7, 0xA5, 0xA2, 0x9E,
    0x9B, 0x98, 0x95, 0x91, 0x8E, 0x8A, 0x87, 0x83, 0x80, 0x7C, 0x78, 0x75, 0x71, 0x6E, 0x6A, 0x67,
    0x64, 0x61, 0x5D, 0x5A, 0x58, 0x55, 0x52, 0x50, 0x4D, 0x4B, 0x49, 0x47, 0x46, 0x44, 0x43, 0x42,
    0x41, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x41, 0x42, 0x43, 0x44, 0x46, 0x47, 0x49, 0x4B,
    0x4D, 0x50, 0x52, 0x55, 0x58, 0x5A, 0x5D, 0x61, 0x64, 0x67, 0x6A, 0x6E, 0x71, 0x75, 0x78, 0x7C
  },
  // Triangular wave table
  {
    0x80, 0x84, 0x89, 0x8D, 0x92, 0x96, 0x9B, 0x9F, 0xA4, 0xA8, 0xAD, 0xB2, 0xB6, 0xBB, 0xBF, 0xC4,
    0xC8, 0xCD, 0xD1, 0xD6, 0xDB, 0xDF, 0xE4, 0xE8, 0xED, 0xF1, 0xF6, 0xFA, 0xFF, 0xFA, 0xF6, 0xF1,
    0xED, 0xE8, 0xE4, 0xDF, 0xDB, 0xD6, 0xD1, 0xCD, 0xC8, 0xC4, 0xBF, 0xBB, 0xB6, 0xB2, 0xAD, 0xA8,
    0xA4, 0x9F, 0x9B, 0x96, 0x92, 0x8D, 0x89, 0x84, 0x7F, 0x7B, 0x76, 0x72, 0x6D, 0x69, 0x64, 0x60,
    0x5B, 0x57, 0x52, 0x4D, 0x49, 0x44, 0x40, 0x3B, 0x37, 0x32, 0x2E, 0x29, 0x24, 0x20, 0x1B, 0x17,
    0x12, 0x0E, 0x09, 0x05, 0x00, 0x05, 0x09, 0x0E, 0x12, 0x17, 0x1B, 0x20, 0x24, 0x29, 0x2E, 0x32,
    0x37, 0x3B, 0x40, 0x44, 0x49, 0x4D, 0x52, 0x57, 0x5B, 0x60, 0x64, 0x69, 0x6D, 0x72, 0x76, 0x7B
  },

  // Sawtooth wave table
  {
    0x00, 0x02, 0x04, 0x06, 0x09, 0x0B, 0x0D, 0x10, 0x12, 0x14, 0x16, 0x19, 0x1B, 0x1D, 0x20, 0x22,
    0x24, 0x27, 0x29, 0x2B, 0x2D, 0x30, 0x32, 0x34, 0x37, 0x39, 0x3B, 0x3E, 0x40, 0x42, 0x44, 0x47,
    0x49, 0x4B, 0x4E, 0x50, 0x52, 0x54, 0x57, 0x59, 0x5B, 0x5E, 0x60, 0x62, 0x65, 0x67, 0x69, 0x6B,
    0x6E, 0x70, 0x72, 0x75, 0x77, 0x79, 0x7C, 0x7E, 0x80, 0x82, 0x85, 0x87, 0x89, 0x8C, 0x8E, 0x90,
    0x93, 0x95, 0x97, 0x99, 0x9C, 0x9E, 0xA0, 0xA3, 0xA5, 0xA7, 0xA9, 0xAC, 0xAE, 0xB0, 0xB3, 0xB5,
    0xB7, 0xBA, 0xBC, 0xBE, 0xC0, 0xC3, 0xC5, 0xC7, 0xCA, 0xCC, 0xCE, 0xD1, 0xD3, 0xD5, 0xD7, 0xDA,
    0xDC, 0xDE, 0xE1, 0xE3, 0xE5, 0xE8, 0xEA, 0xEC, 0xEE, 0xF1, 0xF3, 0xF5, 0xF8, 0xFA, 0xFC, 0xFE
  },
  // Square wave table
  {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  }
};




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

/////// -----------------------------------------------------------

// #define ENCODER_OPTIMIZE_INTERRUPTS // rotary encoder
// // #include <Encoder.h>//rotary encoder
// #include <EEPROM.h>

// // #define D7  13
// // #define D5  14

// #define D7 7
// #define D5 5

// // Encoder myEnc(D10, D9);//rotary encoder
// float oldPosition = -999; // rotary encoder
// float newPosition = -999; // rotary encoder

// float i;        // sample play progress
// float freq = 1; // sample frequency
// bool trig1, old_trig1, done_trig1;
// int sound_out;      // sound out PWM rate
// byte sample_no = 1; // select sample number

// long timer = 0;        // timer count for eeprom write
// bool eeprom_write = 0; // 0=no write,1=write

// const static byte smpl[1][128] PROGMEM = {
//     {0x80, 0x86, 0x8c, 0x92, 0x98, 0x9e, 0xa5, 0xaa,
//      0xb0, 0xb6, 0xbc, 0xc1, 0xc6, 0xcb, 0xd0, 0xd5,
//      0xda, 0xde, 0xe2, 0xe6, 0xea, 0xed, 0xf0, 0xf3,
//      0xf5, 0xf8, 0xfa, 0xfb, 0xfd, 0xfe, 0xfe, 0xff,
//      0xff, 0xff, 0xfe, 0xfe, 0xfd, 0xfb, 0xfa, 0xf8,
//      0xf5, 0xf3, 0xf0, 0xed, 0xea, 0xe6, 0xe2, 0xde,
//      0xda, 0xd5, 0xd0, 0xcb, 0xc6, 0xc1, 0xbc, 0xb6,
//      0xb0, 0xaa, 0xa5, 0x9e, 0x98, 0x92, 0x8c, 0x86,
//      0x80, 0x79, 0x73, 0x6d, 0x67, 0x61, 0x5a, 0x55,
//      0x4f, 0x49, 0x43, 0x3e, 0x39, 0x34, 0x2f, 0x2a,
//      0x25, 0x21, 0x1d, 0x19, 0x15, 0x12, 0xf, 0xc,
//      0xa, 0x7, 0x5, 0x4, 0x2, 0x1, 0x1, 0x0,
//      0x0, 0x0, 0x1, 0x1, 0x2, 0x4, 0x5, 0x7,
//      0xa, 0xc, 0xf, 0x12, 0x15, 0x19, 0x1d, 0x21,
//      0x25, 0x2a, 0x2f, 0x34, 0x39, 0x3e, 0x43, 0x49,
//      0x4f, 0x55, 0x5a, 0x61, 0x67, 0x6d, 0x73, 0x79},
// };

// void eeprom_update()
// {
//   EEPROM.put(0, sample_no);
//   EEPROM.commit();
// }

// //-------------------------timer interrupt for sound----------------------------------
// hw_timer_t *timer0 = NULL;
// portMUX_TYPE timerMux0 = portMUX_INITIALIZER_UNLOCKED;
// volatile uint8_t ledstat = 0;

// void IRAM_ATTR onTimer()
// {
//   portENTER_CRITICAL_ISR(&timerMux0); // enter critical range
//   if (done_trig1 == 1)
//   { // when trigger in
//     i = i + freq;
//     if (i >= 28800)
//     { // when sample playd all ,28800 = 48KHz sampling * 0.6sec
//       i = 0;
//       done_trig1 = 0;
//     }
//   }
//   sound_out = (((pgm_read_byte(&(smpl[sample_no][(int)i * 2]))) | (pgm_read_byte(&(smpl[sample_no][(int)i * 2 + 1]))) << 8) >> 6); // 16bit to 10bit
//   ledcWrite(1, sound_out + 511);                                                                                                   // PWM output
//   portEXIT_CRITICAL_ISR(&timerMux0);                                                                                               // exit critical range
// }

// void setup()
// {
//   EEPROM.begin(1);          // 1byte memory space
//   EEPROM.get(0, sample_no); // callback saved sample number
//   sample_no++;              // countermeasure rotary encoder error
//   if (sample_no >= 48)
//   { // countermeasure rotary encoder error
//     sample_no = 0;
//   }

//   pinMode(D7, INPUT); // trigger in
//   // pinMode(D9, INPUT_PULLUP); //rotary encoder
//   // pinMode(D10, INPUT_PULLUP); //rotary encoder
//   pinMode(D5, OUTPUT); // sound_out PWM
//   timer = millis();    // for eeprom write
//   analogReadResolution(10);

//   ledcSetup(1, 39000, 10); // PWM frequency and resolution
//   ledcAttachPin(D5, 1);    //(LED_PIN, LEDC_CHANNEL_0);//timer ch1 , apply D5 output

//   timer0 = timerBegin(0, 1666, true);           // timer0, 12.5ns*1666 = 20.83usec(48kHz), count-up
//   timerAttachInterrupt(timer0, &onTimer, true); // edge-triggered
//   timerAlarmWrite(timer0, 1, true);             // 1*20.83usec = 20.83usec, auto-reload
//   timerAlarmEnable(timer0);                     // enable timer0
// }

// void loop()
// {
//   //-------------------------trigger----------------------------------
//   old_trig1 = trig1;
//   trig1 = digitalRead(D7);
//   if (trig1 == 1 && old_trig1 == 0)
//   { // detect trigger signal low to high , before sample play was done
//     done_trig1 = 1;
//     i = 0;
//   }

//   //-------------------------pitch setting----------------------------------
//   freq = analogRead(A3) * 0.002 + analogRead(A0) * 0.002;

//   //-------------------------sample change----------------------------------
//   // newPosition = myEnc.read();
//   // if ( (newPosition - 3) / 4  > oldPosition / 4) {
//   //   oldPosition = newPosition;
//   //   sample_no = sample_no - 1;
//   //   if (sample_no < 0 || sample_no > 200) {//>200 is overflow countermeasure
//   //     sample_no = 47;
//   //   }
//   //   done_trig1 = 1;//1 shot play when sample changed
//   //   i = 0;
//   //   timer = millis();
//   //   eeprom_write = 1;//eeprom update flug on
//   // }

//   // else if ( (newPosition + 3) / 4  < oldPosition / 4 ) {
//   //   oldPosition = newPosition;
//   //   sample_no = sample_no + 1;
//   //   if (sample_no >= 48) {
//   //     sample_no = 0;
//   //   }
//   //   done_trig1 = 1;//1 shot play when sample changed
//   //   i = 0;
//   //   timer = millis();
//   //   eeprom_write = 1;//eeprom update flug on
//   // }

//   //-------------------------save to eeprom----------------------------------
//   if (timer + 5000 <= millis() && eeprom_write == 1)
//   { // Memorized 5 seconds after sample number change
//     eeprom_write = 0;
//     eeprom_update();
//   }
// }


// //////////////////////////-----------------------------

// /*
//   Setup base wave form @ 152,380 Hz using the ESP32's hardware PWM
//       152,380 Hz is the fastest oscilation its hardware PWM can achieve with at least 9 bit of resolution
//         Get this with: clk_src = LEDC_APB_CLK (80 MHz).
//         ... then duty resolution is integer (log 2 (LEDC_APB_CLK / frequency))
//       see: 
//         https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/ledc.html
//         https://github.com/espressif/esp-idf/blob/5893797bf068ba6f72105fff289ead370b4591a3/examples/peripherals/ledc/ledc_basic/main/ledc_basic_example_main.c
//   Initial code to get PWM working from: https://randomnerdtutorials.com/esp32-pwm-arduino-ide/
//   Note: We really want this PWM to run in LEDC_HIGH_SPEED_MODE; the code from the esp-idf gives this control, but not the code for arduino;
//     Currently; assuming arduino puts PWM into high-speed mode by default
// */

// // #define DACwave // uncomment to use DAC to generate base pulse wave instead of ttPWM

// const int update_rate = 1000; // Controls both sound updates and when to process sensor data/ update motion models

// // Variables controlling base pulse wave production / volume control
// #ifdef DACwave
// const int soundPin = A1;  // DAC is set to use DAC1, which is A1; DAC2 is A0
// const int amp_max = 255; // DAC is 8-bit, so only 256 levels
// #else 
// const int soundPin = 13; 
// const int base_wave_freq = 152380; // highest availabe with 9 bit resolution is 152380
// const int pwm_Channel = 5;
// const int pwm_resolution = (int) log2( 80000000 / base_wave_freq ) ;
// const int amp_max = pow(2,pwm_resolution) - 1; // for 9 bit resolution: 2^9 - 1 = 511; for 8 bit 2^8 - 1 = 255
// #endif
// volatile int amp = (int) amp_max * 0.5;
// // setting the timer for generating hearable oscilation 
// const int pitch_initial = 440;
// volatile int pitch = pitch_initial;
// hw_timer_t * timerSW = NULL;
// portMUX_TYPE timerSWMux = portMUX_INITIALIZER_UNLOCKED;
// const int timerSWprescaler = 2; // Timers are 80Mhz; counters seem to be at least 32bit, so, don't need to prescale down low
// const long timerSize = 80000000;
// const int timerSizeprescaler = timerSize/timerSWprescaler;
// volatile int timerSW_top = (int) timerSizeprescaler / (pitch * 2); // controls frequency of hearable oscilation
// volatile bool up = true;

// void update_pitch ( int new_pitch ) {
//   timerSW_top = (int) timerSizeprescaler / (new_pitch * 2);
//   timerAlarmWrite(timerSW, timerSW_top, true); // true = reload automatically
// }

// // interrupt handlers for timers generating hearable oscilation: 
// void IRAM_ATTR onTimerSW() {
//   portENTER_CRITICAL_ISR(&timerSWMux);
//   #ifdef DACwave
//     if (up) {
//       dacWrite(DAC1, amp);
//     } else {
//       dacWrite(DAC1, 0);
//     }
//   #else 
//     if (up) {
//       ledcWrite(pwm_Channel, amp);
//     } else {
//       ledcWrite(pwm_Channel, 0);
//     }
//   #endif
//   up = !up;
//   portEXIT_CRITICAL_ISR(&timerSWMux);
// }

// // Variables for the timer controlling sound updates / motion processing
// hw_timer_t * timerSU = NULL;
// portMUX_TYPE timerSUMux = portMUX_INITIALIZER_UNLOCKED;
// const int timerSU_top = (int) timerSizeprescaler / update_rate; 
// volatile bool update_now = false;

// // interrupt handler for timer generating sound updates / motion processing: 
// void IRAM_ATTR onTimerSU() {
//   portENTER_CRITICAL_ISR(&timerSWMux);
//   update_now = true;
//   portEXIT_CRITICAL_ISR(&timerSWMux);
// }
 
// void setup(){

//   // Setup base pulse wav
//   #ifdef DACwave
//     // print notice that DAC is used; nothing else to run
//   #else
//     // print notice that ttPWM is beeing used
//     ledcSetup(pwm_Channel, base_wave_freq, pwm_resolution); // configure PWM functionalitites
//     ledcAttachPin(soundPin, pwm_Channel); // attach the channel to the pin generating the wave
//   #endif
//   // Setup timer used for hearable oscilation
//   timerSW = timerBegin(0, timerSWprescaler, true); // true indicates counter goes up
//   timerAttachInterrupt(timerSW, &onTimerSW, true); // true indicates edge interrupt (false for level)
//   timerAlarmWrite(timerSW, timerSW_top, true); // true for reload automatically
//   timerAlarmEnable(timerSW);
//   // Setup timer for sound updates/ motion processing
//   timerSU = timerBegin(1, timerSWprescaler, true); // true indicates counter goes up
//   timerAttachInterrupt(timerSU, &onTimerSU, true); // true indicates edge interrupt (false for level)
//   timerAlarmWrite(timerSU, timerSU_top, true); // true for reload automatically
//   timerAlarmEnable(timerSU);

//   amp = amp_max;
// }
 
// void loop(){  

//   if (update_now) {
//     if ( pitch < 2000 ) pitch += 1;
//     else pitch = pitch_initial;
//     update_pitch(pitch);

//     if ( amp < 5 ) amp = amp_max;
//     else amp -= 1;

//     update_now = false;
//   }
  
// }


// /////////////////////-----------------------------

// // the number of the LED pin
// const int ledPin = 3;

// // setting PWM properties
// const int freq = 5000;
// const int ledChannel = 0;
// const int resolution = 8;

// void setup()
// {
//   // configure LED PWM functionalitites
//   ledcSetup(ledChannel, freq, resolution);

//   // attach the channel to the GPIO to be controlled
//   ledcAttachPin(ledPin, ledChannel);
// }

// void loop()
// {
//   // https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-ledc.c

//   ledcWriteNote(ledChannel, NOTE_C, 4);
//   delay(500);
//   ledcWriteTone(ledChannel, 800);
//   delay(500);
// }


// /////////////////////-----------------------------

// // the number of the LED pin
// const int ledPin = 3;

// // setting PWM properties
// const int freq = 5000;
// const int ledChannel = 0;
// const int resolution = 8;

// void setup()
// {
//   // configure LED PWM functionalitites
//   ledcSetup(ledChannel, freq, resolution);

//   // attach the channel to the GPIO to be controlled
//   ledcAttachPin(ledPin, ledChannel);
// }

// void loop()
// {
//   // increase the LED brightness
//   for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle++)
//   {
//     // changing the LED brightness with PWM
//     ledcWrite(ledChannel, dutyCycle);
//     delay(15);
//   }

//   // decrease the LED brightness
//   for (int dutyCycle = 255; dutyCycle >= 0; dutyCycle--)
//   {
//     // changing the LED brightness with PWM
//     ledcWrite(ledChannel, dutyCycle);
//     delay(15);
//   }
// }

///////////-----------------------------

// the number of the LED pin
const int ledPin = 3;

// setting PWM properties
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

void setup()
{
  // configure LED PWM functionalitites
  ledcSetup(ledChannel, freq, resolution);

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(ledPin, ledChannel);
}

void loop()
{
  // increase the LED brightness
  for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle++)
  {
    // changing the LED brightness with PWM
    ledcWrite(ledChannel, dutyCycle);
    delay(15);
  }

  // decrease the LED brightness
  for (int dutyCycle = 255; dutyCycle >= 0; dutyCycle--)
  {
    // changing the LED brightness with PWM
    ledcWrite(ledChannel, dutyCycle);
    delay(15);
  }
}

