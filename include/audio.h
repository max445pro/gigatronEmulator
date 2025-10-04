#ifndef AUDIO_H
#define AUDIO_H

#include <windows.h>
#include <mmsystem.h>
#include <stdint.h>

typedef struct {
    HWAVEOUT hWaveOut;
    WAVEFORMATEX wfx;
    WAVEHDR header;
} AudioPlayer;

AudioPlayer initAudioPlayer();
uint8_t convert4to8(uint8_t sample4);

#endif
