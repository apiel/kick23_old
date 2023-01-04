#ifndef _DEF_H_
#define _DEF_H_

// For linux runtime compatibility
#ifndef IRAM_ATTR
#define IRAM_ATTR
#endif

#define SAMPLE_RATE 44100
const float SAMPLE_PER_MS = SAMPLE_RATE / 1000.0f;

#endif