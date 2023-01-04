#include "./sdl2.h"

#include <stdint.h>

#include "../src/synth.h"

#ifndef APP_LOG
#define APP_LOG printf
#endif

#define SCREEN_W 480
#define SCREEN_H 320

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

    while (handleEvent()) {
        triggerSound();
        SDL_Delay(2000);
    }

    SDL_DestroyWindow(window);
    SDL_CloseAudioDevice(audioDevice);

    SDL_Quit();
    return 0;
}