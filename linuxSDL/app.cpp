#include <SDL2/SDL.h>

#include <stdint.h>

#ifndef APP_LOG
#define APP_LOG printf
#endif

#include "../src/synth.h"

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

SDL_Rect pot1 = { 10, 10, 20, 220 };
SDL_Rect pot2 = { 40, 10, 20, 220 };

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
        } else {
            buttonReleased();
        }
        break;
    case 4: // A
        if (event->type == SDL_KEYDOWN) {
            rotaryChanged(-1);
        }
        break;
    case 22: // S
    if (event->type == SDL_KEYDOWN) {
            rotaryPressed();
        }
        break;
    case 7: // D
        if (event->type == SDL_KEYDOWN) {
            rotaryChanged(+1);
        }
        break;
    }
    default:
        break;
    }

    return true;
}

bool handleMouse(SDL_MouseButtonEvent* event)
{
    // SDL_Log("handleMouse %d %d %d %d\n", event->button, event->x, event->y, event->state);
    SDL_Point mousePosition;
    mousePosition.x = event->x;
    mousePosition.y = event->y;

    if (SDL_PointInRect(&mousePosition, &pot1)) {
        uint8_t value = (float)(event->y - pot1.y) / (float)(pot1.h - pot1.y) * 100;
        if (value > 100)
            value = 100;
        // APP_LOG("pot1 %d\n", value);
        updatePot(0, value);
    } else if (SDL_PointInRect(&mousePosition, &pot2)) {
        uint8_t value = (float)(event->y - pot2.y) / (float)(pot2.h - pot2.y) * 100;
        if (value > 100)
            value = 100;
        // APP_LOG("pot2 %d\n", value);
        updatePot(1, value);
    }
    return true;
}

bool mouseBtnDown = false;
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

        case SDL_MOUSEBUTTONDOWN: {
            mouseBtnDown = true;
            handleMouse(&event.button);
            break;
        }

        case SDL_MOUSEBUTTONUP: {
            mouseBtnDown = false;
            break;
        }

        case SDL_MOUSEMOTION: {
            if (mouseBtnDown) {
                handleMouse(&event.button);
            }
            break;
        }
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
    SDL_Surface* screenSurface = SDL_GetWindowSurface(window);

    SDL_AudioDeviceID audioDevice = initAudio(audioCallBack);
    if (SDL_getenv("APP_SKIP_AUDIO") == NULL && !audioDevice) {
        return 1;
    }

    SDL_UpdateWindowSurface(window);

    // uint8_t counter = 0;
    while (handleEvent()) {
        // if (counter < 3) {
        //     printf("Trigger sound\n");
        //     triggerSound();
        //     SDL_Delay(1000);
        //     counter++;
        // }

        Uint32 color = SDL_MapRGB(screenSurface->format, 255, 255, 255);

        SDL_FillRect(screenSurface, &pot1, color);
        SDL_FillRect(screenSurface, &pot2, color);

        SDL_UpdateWindowSurface(window);
        // SDL_Delay(10);
    }

    SDL_DestroyWindow(window);
    SDL_CloseAudioDevice(audioDevice);

    SDL_Quit();
    return 0;
}