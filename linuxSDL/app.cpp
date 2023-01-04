#include <SDL2/SDL.h>

#include <stdint.h>

#include "../src/synth.h"

#ifndef APP_LOG
#define APP_LOG printf
#endif

#define SCREEN_W 480
#define SCREEN_H 320

#ifndef SAMPLE_RATE
#define SAMPLE_RATE 44100
#endif

#ifndef APP_CHANNELS
#define APP_CHANNELS 2
#endif

#ifndef APP_AUDIO_FORMAT
#define APP_AUDIO_FORMAT AUDIO_S16LSB
// #define APP_AUDIO_FORMAT AUDIO_F32LSB
#endif

#ifndef APP_AUDIO_CHUNK
// #define APP_AUDIO_CHUNK 1024
#define APP_AUDIO_CHUNK 128
#endif

bool handleKeyboard(SDL_KeyboardEvent* event)
{
    SDL_Log("handleKeyboard %d\n", event->keysym.scancode);
    // SDL_Log("handleKeyboard %d\n", event->repeat);
    switch (event->keysym.scancode) {
    case SDL_SCANCODE_ESCAPE:
        return false;
    case SDL_SCANCODE_SPACE: {
        if (event->type == SDL_KEYDOWN) {
            buttonPressed();
        }
        break;
    }
    default:
        break;
    }

    return true;
}

bool handleEvent()
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        // if (event.type > 0x300 && event.type < 0x800) {
        //     SDL_Log("handleEvent %d\n", event.type);
        // }

        switch (event.type) {
        case SDL_QUIT:
            return false;
        case SDL_KEYUP:
        case SDL_KEYDOWN:
            return handleKeyboard(&event.key);
        }
    }

    return true;
}

SDL_AudioDeviceID initAudio(SDL_AudioCallback callback)
{
    SDL_AudioSpec spec, aspec;

    SDL_zero(spec);
    spec.freq = SAMPLE_RATE;
    spec.format = APP_AUDIO_FORMAT;
    spec.channels = APP_CHANNELS;
    spec.samples = APP_AUDIO_CHUNK;
    spec.callback = callback;
    spec.userdata = NULL;

    SDL_AudioDeviceID audioDevice = SDL_OpenAudioDevice(NULL, 0, &spec, &aspec, SDL_AUDIO_ALLOW_ANY_CHANGE);
    if (audioDevice <= 0) {
        fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
        return false;
    }

    SDL_Log("aspec freq %d channel %d sample %d format %d", aspec.freq, aspec.channels, aspec.samples, aspec.format);

    // Start playing, "unpause"
    SDL_PauseAudioDevice(audioDevice, 0);
    return audioDevice;
}

void audioCallBack(void* userdata, Uint8* stream, int len)
{
    static union sampleTUNT {
        Uint8 ch[2];
        int16_t sample;
    } sampleDataU;

    for (int i = 0; i < len; i++) {
        sampleDataU.sample = (int16_t)(getSample() * 32767);
        // printf("%d\n", sampleDataU.sample);
        stream[i] = sampleDataU.ch[0];
        i++;
        stream[i] = sampleDataU.ch[1];

#if APP_CHANNELS == 2
        i++;
        stream[i] = sampleDataU.ch[0];
        i++;
        stream[i] = sampleDataU.ch[1];
#endif
    }
}

int main(int argc, char* args[])
{
    SDL_Log(">>>>>>> Start APP\n");

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "App",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_W, SCREEN_H,
        SDL_WINDOW_SHOWN);

    if (window == NULL) {
        fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
        return 1;
    }
    // SDL_Surface* screenSurface = SDL_GetWindowSurface(window);

    SDL_AudioDeviceID audioDevice = initAudio(audioCallBack);
    if (SDL_getenv("APP_SKIP_AUDIO") == NULL && !audioDevice) {
        return 1;
    }

    SDL_UpdateWindowSurface(window);

    uint8_t counter = 0;
    while (handleEvent()) {
        if (counter < 3) {
            printf("Trigger sound\n");
            triggerSound();
            SDL_Delay(1000);
            counter++;
        }
        // SDL_Delay(10);
    }

    SDL_DestroyWindow(window);
    SDL_CloseAudioDevice(audioDevice);

    SDL_Quit();
    return 0;
}