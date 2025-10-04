#include <windows.h>
#include <mmsystem.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

// Link with winmm.lib when compiling
// (cl myprog.c /link winmm.lib)

int main() {
    HWAVEOUT hWaveOut;
    WAVEFORMATEX wfx;

    // Set audio format: 8-bit unsigned, mono, 44100 Hz
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 1;
    wfx.nSamplesPerSec = 44100;
    wfx.wBitsPerSample = 8;
    wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;

    // Open default audio device
    if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
        printf("Failed to open audio\n");
        return 1;
    }

    // Make some fake 4-bit samples (a sawtooth wave)
    int samples = 44100; // 1 second
    uint8_t *buffer = malloc(samples);
    for (int i = 0; i < samples; i++) {
        uint8_t sample4 = sinf((float)i) v;
        buffer[i] = (sample4 << 4) | sample4; // expand to 8-bit
    }

    // Prepare header
    WAVEHDR header;
    header.lpData = (LPSTR)buffer;
    header.dwBufferLength = samples;
    header.dwFlags = 0;
    header.dwLoops = 0;

    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));

    // Wait until done
    Sleep(1000);

    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
    free(buffer);
    return 0;
}
