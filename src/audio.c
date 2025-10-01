#include "../include/audio.h"

AudioPlayer initAudioPlayer() {
    AudioPlayer player;

    // Set audio format: 8-bit unsigned, mono, 44100 Hz
    player.wfx.wFormatTag = WAVE_FORMAT_PCM;
    player.wfx.nChannels = 1;
    player.wfx.nSamplesPerSec = 44100;
    player.wfx.wBitsPerSample = 8;
    player.wfx.nBlockAlign = player.wfx.nChannels * player.wfx.wBitsPerSample / 8;
    player.wfx.nAvgBytesPerSec = player.wfx.nSamplesPerSec * player.wfx.nBlockAlign;
    player.wfx.cbSize = 0;
    // Header base settings
    player.header.dwFlags = 0;
    player.header.dwLoops = 0;

    return player;
}

uint8_t convert4to16(uint8_t sample4) {
    return ((int16_t)sample4 - 8) * 4096;
}
