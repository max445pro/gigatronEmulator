#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "include/glfw3.h"
#include "include/core.h"
#include "include/keyboard.h"
#include "include/audio.h"

uint8_t SINE_TABLE[]=
{
  128, 131, 134, 137, 140, 143, 146, 149, 152, 156, 159, 162, 165, 168, 171, 174,
  176, 179, 182, 185, 188, 191, 193, 196, 199, 201, 204, 206, 209, 211, 213, 216,
  218, 220, 222, 224, 226, 228, 230, 232, 234, 236, 237, 239, 240, 242, 243, 245,
  246, 247, 248, 249, 250, 251, 252, 252, 253, 254, 254, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 254, 254, 253, 252, 252, 251, 250, 249, 248, 247,
  246, 245, 243, 242, 240, 239, 237, 236, 234, 232, 230, 228, 226, 224, 222, 220,
  218, 216, 213, 211, 209, 206, 204, 201, 199, 196, 193, 191, 188, 185, 182, 179,
  176, 174, 171, 168, 165, 162, 159, 156, 152, 149, 146, 143, 140, 137, 134, 131,
  127, 124, 121, 118, 115, 112, 109, 106, 103, 99, 96, 93, 90, 87, 84, 81,
  79, 76, 73, 70, 67, 64, 62, 59, 56, 54, 51, 49, 46, 44, 42, 39,
  37, 35, 33, 31, 29, 27, 25, 23, 21, 19, 18, 16, 15, 13, 12, 10,
  9, 8, 7, 6, 5, 4, 3, 3, 2, 1, 1, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 3, 4, 5, 6, 7, 8,
  9, 10, 12, 13, 15, 16, 18, 19, 21, 23, 25, 27, 29, 31, 33, 35,
  37, 39, 42, 44, 46, 49, 51, 54, 56, 59, 62, 64, 67, 70, 73, 76,
  79, 81, 84, 87, 90, 93, 96, 99, 103, 106, 109, 112, 115, 118, 121, 124
};

#define WIDTH       160.0
#define HEIGHT      120.0
#define pixelScale  4.0     // Anything other then four dont work yet
int GLSW = (int)(WIDTH*pixelScale);
int GLSH = (int)(HEIGHT*pixelScale);

const uint8_t rgb2bitTo8bit[4] = {0, 85, 170, 255};
void pixel(int x, int y, uint8_t color) {
    //if ((color & 0x3f) != 0) printf("color: %02x X:%d Y:%d\n", color & 0x3f, x, y);
    glColor3ub(rgb2bitTo8bit[(color & 0b00000011)],
               rgb2bitTo8bit[((color & 0b00001100) >> 2)],
               rgb2bitTo8bit[((color & 0b00110000) >> 4)]);
    glPointSize(pixelScale);
    glBegin(GL_POINTS);
    glVertex2i(x * pixelScale - 50, y + 3);
    glEnd();
}

void clearBackground() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            pixel(x, y, x);
        }
    }
}

void display(GLFWwindow *window) {
    //glClear(GL_COLOR_BUFFER_BIT);

    //printf("\n\n\n\n\n\n\nok\n");
    glfwSwapBuffers(window);
}

void load_rom_file(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Cannot open rom file\n");
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);

    rewind(file);

    fread(&ROM, 1, file_size, file);

    fclose(file);
}

float deltaTime = 0.0f;
float lastFrame = 0.0f;
int delayprint = 10;

int main(int argc, char *argv[]) {
    // GLFW init
    if (!glfwInit()) return -1;

    GLFWwindow *window = glfwCreateWindow(GLSW, GLSH, "Gigatron", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwGetFramebufferSize(window, &GLSW, &GLSH);
    glfwSetKeyCallback(window, key_callback);

    // Screen setup
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, GLSW, GLSH, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Gigatron setup
    CpuState S;
    //AudioPlayer player = initAudioPlayer();
    load_rom_file("roms/ROMv6.rom");
    srand(time(NULL));

    // Sound setup
    HWAVEOUT hWaveOut;
    WAVEFORMATEX wfx;

    #define SAMPLES 256
    uint8_t bufferA[SAMPLES];
    uint8_t bufferB[SAMPLES];
    uint8_t bufferC[SAMPLES];
    WAVEHDR headerA = {0};
    WAVEHDR headerB = {0};
    WAVEHDR headerC = {0};
    WAVEHDR *current_hdr = &headerA;
    headerA.lpData = (LPSTR)bufferA;
    headerA.dwBufferLength = SAMPLES;
    headerA.dwFlags = 0;
    headerA.dwLoops = 0;

    headerB.lpData = (LPSTR)bufferB;
    headerB.dwBufferLength = SAMPLES;
    headerB.dwFlags = 0;
    headerB.dwLoops = 0;

    headerC.lpData = (LPSTR)bufferB;
    headerC.dwBufferLength = SAMPLES;
    headerC.dwFlags = 0;
    headerC.dwLoops = 0;

    
    uint16_t audio_pos = 0;

    // Set audio format: 8-bit unsigned, mono, 44100 Hz
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 1;
    wfx.nSamplesPerSec = 4096;
    wfx.wBitsPerSample = 8;
    wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;

    // Open default audio device
    if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
        printf("Failed to open audio\n");
        return 1;
    }

    // Main loop
    int vgaX = 0, vgaY = 0;
    float frame_start = glfwGetTime();
    for (long long t = -2; !glfwWindowShouldClose(window); t++) {
        if (t < 0) S.PC = 0;

        CpuState T = cycle(S);
        //printf("IR:%02x, D:%02x, %04x, %02x\n", T.IR, T.D, T.PC, T.AC);

        int hSync = (T.OUT_PORT & 0x40) - (S.OUT_PORT & 0x40);
        int vSync = (T.OUT_PORT & 0x80) - (S.OUT_PORT & 0x80);

        if (vSync < 0) {
            vgaY = -36;  // Falling vSync Edge

            // FPS calculation
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            // Update last frame time for the next iteration
            lastFrame = currentFrame;

            // Realtime regulation
            if (!turbo) { while (glfwGetTime() - frame_start < 1.0/60.0); }    // Adjust for real gigatron speed
            frame_start = glfwGetTime();

            display(window);
            glfwPollEvents();
 
        }

        if (vgaX++ < 200) {
            pixel(vgaX, vgaY, S.OUT_PORT & 0x3F);  // Plot pixel
        } 

        if (hSync > 0) { // Rising hSync edge
            vgaX = 0;
            vgaY++;
            T.undef = rand() & 0xff;

            // Update audio 
            if (vgaY % 4 == 0) {
                //uint8_t pcm = convert4to8( SINE_TABLE[(audio_pos * 8) % 256] >> 4 );
                uint8_t pcm = convert4to8( (T.AC & 0xf0) >> 4 );
                ((uint8_t*)current_hdr->lpData)[audio_pos++] = pcm;

                if (audio_pos >= SAMPLES) {

                    waveOutPrepareHeader(hWaveOut, current_hdr, sizeof(WAVEHDR));
                    waveOutWrite(hWaveOut, current_hdr, sizeof(WAVEHDR));

                    // Switch to the other buffer
                    if (current_hdr == &headerA) {
                        current_hdr = &headerB;
                        current_hdr->lpData = (LPSTR)bufferB;
                    } else if (current_hdr == &headerB) {
                        current_hdr = &headerC;
                        current_hdr->lpData = (LPSTR)bufferC;
                    } else {
                        current_hdr = &headerA;
                        current_hdr->lpData = (LPSTR)bufferA;
                    }

                    audio_pos = 0;
                }
            }
        }

        if (headerA.dwFlags & WHDR_DONE) {
            waveOutUnprepareHeader(hWaveOut, &headerA, sizeof(WAVEHDR));
            // Now refill bufferA
        }
        if (headerB.dwFlags & WHDR_DONE) {
            waveOutUnprepareHeader(hWaveOut, &headerB, sizeof(WAVEHDR));
            // Now refill bufferA
        }

        //Too much overhead
        keyboard_handler(window);
        S = T;
    }

    waveOutClose(hWaveOut);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
