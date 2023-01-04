#ifndef APP_SDL2_H_
#define APP_SDL2_H_

#include <SDL2/SDL.h>

#include <stdio.h>

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

#endif