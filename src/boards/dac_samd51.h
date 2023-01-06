#ifndef AUDIO_DAC_H
#define AUDIO_DAC_H

#include <Arduino.h>

#include <Adafruit_ZeroDMA.h>
#include <wiring_private.h>

#define AUDIO_TC TC2
#define AUDIO_IRQn TC2_IRQn
#define AUDIO_Handler TC2_Handler
#define AUDIO_GCLK_ID TC2_GCLK_ID
#define AUDIO_TC_DMAC_ID_OVF TC2_DMAC_ID_OVF

#define WAIT_TC8_REGS_SYNC(x)                                             \
    while (x->COUNT8.SYNCBUSY.bit.ENABLE || x->COUNT8.SYNCBUSY.bit.SWRST) \
        ;

#define AUDIO_CLKRATE (VARIANT_GCLK2_FREQ >> 4)
#define AUDIO_PRESCALER TC_CTRLA_PRESCALER_DIV16
#define AUDIO_TC_FREQ 44100
#define AUDIO_BLOCK_SAMPLES 128

static uint32_t dac_buffer[AUDIO_BLOCK_SAMPLES * 2];

static Adafruit_ZeroDMA* dma0;
static DmacDescriptor* desc;
static ZeroDMAstatus stat;
static uint32_t* saddr;

typedef struct audio_block_struct {
    uint8_t ref_count;
    uint8_t reserved1;
    uint16_t memory_pool_index;
    int16_t data[AUDIO_BLOCK_SAMPLES];
} audio_block_t;

static audio_block_t block_silent;

void defaultCbIsrDac(const uint32_t* end, uint32_t* dest)
{
    do {
        *dest++ = 0;
        *dest++ = 0;
    } while (dest < end);
}

void (*callbackIsrDac)(const uint32_t* end, uint32_t* dest) = defaultCbIsrDac;

void isrDac(Adafruit_ZeroDMA* dma)
{
    const uint32_t* end;
    uint32_t* dest;

    if (saddr == dac_buffer) {
        // DMA has finished the first half
        dest = dac_buffer;
        end = dac_buffer + AUDIO_BLOCK_SAMPLES;
        saddr = (uint32_t*)end;
    } else {
        // DMA has finished the second half
        dest = dac_buffer + AUDIO_BLOCK_SAMPLES;
        end = dac_buffer + AUDIO_BLOCK_SAMPLES * 2;
        saddr = dac_buffer;
    }

    callbackIsrDac(end, dest);
}

/**
 * @brief Initialize the DAC and DMA for audio output
 * 
 * @param cb callback function to generate the audio output
 */
void initDac(void (*cb)(const uint32_t* end, uint32_t* dest) = defaultCbIsrDac)
{
    callbackIsrDac = cb;

    dma0 = new Adafruit_ZeroDMA();
    stat = dma0->allocate();

    pinPeripheral(PIN_DAC0, PIO_ANALOG);
    pinPeripheral(PIN_DAC1, PIO_ANALOG);

    while (DAC->SYNCBUSY.bit.ENABLE || DAC->SYNCBUSY.bit.SWRST)
        ;
    DAC->CTRLA.bit.ENABLE = 0; // disable DAC

    while (DAC->SYNCBUSY.bit.ENABLE || DAC->SYNCBUSY.bit.SWRST)
        ;
    DAC->DACCTRL[0].bit.ENABLE = 1;
    DAC->DACCTRL[1].bit.ENABLE = 1;

    while (DAC->SYNCBUSY.bit.ENABLE || DAC->SYNCBUSY.bit.SWRST)
        ;
    DAC->CTRLA.bit.ENABLE = 1; // enable DAC

    memset(&block_silent, 0, sizeof(block_silent));

    // slowly ramp up to DC voltage, approx 1/4 second
    for (int16_t i = 0; i <= 2048; i += 8) {
        while (!DAC->STATUS.bit.READY0 || !DAC->STATUS.bit.READY1)
            ;
        while (DAC->SYNCBUSY.bit.DATA0 || DAC->SYNCBUSY.bit.DATA1)
            ;
        DAC->DATA[0].reg = i;
        DAC->DATA[1].reg = i;
        delay(1);
    }

    // TODO: on SAMD51 lets find an unused timer and use that
    GCLK->PCHCTRL[AUDIO_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK2_Val | (1 << GCLK_PCHCTRL_CHEN_Pos);
    AUDIO_TC->COUNT8.WAVE.reg = TC_WAVE_WAVEGEN_NFRQ;

    AUDIO_TC->COUNT8.CTRLA.reg &= ~TC_CTRLA_ENABLE;
    WAIT_TC8_REGS_SYNC(AUDIO_TC)

    AUDIO_TC->COUNT8.CTRLA.reg = TC_CTRLA_SWRST;
    WAIT_TC8_REGS_SYNC(AUDIO_TC)
    while (AUDIO_TC->COUNT8.CTRLA.bit.SWRST)
        ;

    AUDIO_TC->COUNT8.CTRLA.reg |= TC_CTRLA_MODE_COUNT8 | AUDIO_PRESCALER;
    WAIT_TC8_REGS_SYNC(AUDIO_TC)

    AUDIO_TC->COUNT8.PER.reg = (uint8_t)(AUDIO_CLKRATE / AUDIO_TC_FREQ);
    WAIT_TC8_REGS_SYNC(AUDIO_TC)

    AUDIO_TC->COUNT8.CTRLA.reg |= TC_CTRLA_ENABLE;
    WAIT_TC8_REGS_SYNC(AUDIO_TC)

    dma0->setTrigger(AUDIO_TC_DMAC_ID_OVF);
    dma0->setAction(DMA_TRIGGER_ACTON_BEAT);

    desc = dma0->addDescriptor(
        dac_buffer, // move data from here
        (void*)(&DAC->DATA[0]), // to here
        AUDIO_BLOCK_SAMPLES, // this many...
        DMA_BEAT_SIZE_WORD, // bytes/hword/words
        true, // increment source addr?
        false);
    desc->BTCTRL.bit.BLOCKACT = DMA_BLOCK_ACTION_INT;

    desc = dma0->addDescriptor(
        dac_buffer + AUDIO_BLOCK_SAMPLES, // move data from here
        (void*)(&DAC->DATA[0]), // to here
        AUDIO_BLOCK_SAMPLES, // this many...
        DMA_BEAT_SIZE_WORD, // bytes/hword/words
        true, // increment source addr?
        false);
    desc->BTCTRL.bit.BLOCKACT = DMA_BLOCK_ACTION_INT;
    dma0->loop(true);

    saddr = dac_buffer;

    dma0->setCallback(isrDac);
    dma0->startJob();
}

#endif