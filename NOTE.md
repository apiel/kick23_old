# esp32

To fix noise issue due to pwm, maybe use small filter on output pin:

- https://note.com/solder_state/n/ne33e32660d50
- https://note.com/solder_state/n/n950ed088c3cb
- https://note.com/solder_state/n/nca6a1dec3921

I2S dac

- MAX98357A

https://dronebotworkshop.com/esp32-i2s/


I2C

- custom pin https://randomnerdtutorials.com/esp32-i2c-communication-arduino-ide/#:~:text=have%20more%20flexibility-,Connecting%20I2C%20Devices%20with%20ESP32,the%20SCL%20line%20as%20SCK.

- use i2c to mix audio between each device and the mixer module could have a dac...
    - com between 2 esp32 https://microcontrollerslab.com/esp32-i2c-communication-tutorial-arduino-ide/
    - but goes against modular module to get output and bring to another module, e.g. filter or effect...

# stm32

stm32f4 has 12 bit dac

https://pallavaggarwal.in/2020/03/13/start-learning-stm32f4-mcu-programming/

# rp2040

board:
- RP2040-Zero Mini
- https://www.waveshare.com/wiki/RP2040-Zero
- https://eckstein-shop.de/WaveShare-RP2040-Zero-MCU-Board-Mini-Version-EN
