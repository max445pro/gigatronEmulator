#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "include/glfw3.h"
#include "include/core.h"
#include "include/keyboard.h"
#include "include/audio.h"

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

int main(int argc, char *argv[]) {
    if (!glfwInit()) return -1;

    GLFWwindow *window = glfwCreateWindow(GLSW, GLSH, "Gigatron", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwGetFramebufferSize(window, &GLSW, &GLSH);
    glfwSetKeyCallback(window, key_callback);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, GLSW, GLSH, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    CpuState S;
    AudioPlayer player = initAudioPlayer();
    load_rom_file("roms/ROMv6.rom");
    srand(time(NULL));

    // Sound
    const uint8_t cycles_per_sample = 6250000 / 44100;
    long long next_sample_time = 0;

    const int samples = 44100;  // 1 sec
    uint16_t *buffer = malloc(sizeof(uint16_t) * samples);
    uint16_t audio_pos = 0;

    // Main loop
    int vgaX = 0, vgaY = 0;
    for (long long t = -2; !glfwWindowShouldClose(window); t++) {
        if (t < 0) S.PC = 0;
        float t1 = glfwGetTime();

        CpuState T = cycle(S);
        //printf("IR:%02x, D:%02x, %04x, %02x\n", T.IR, T.D, T.PC, T.AC);

        int hSync = (T.OUT_PORT & 0x40) - (S.OUT_PORT & 0x40);
        int vSync = (T.OUT_PORT & 0x80) - (S.OUT_PORT & 0x80);

        if (vSync < 0) {
            vgaY = -36;  // Falling vSync Edge
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
        }

        if (t >= next_sample_time) {
            int16_t pcm = convert4to16(rand() & 0x0f);
            buffer[audio_pos++] = pcm;
            next_sample_time += cycles_per_sample;

            if (audio_pos >= samples) {
                player.header.lpData = (LPSTR)buffer;
                player.header.dwBufferLength = samples * sizeof(uint16_t);

                waveOutPrepareHeader(player.hWaveOut, &player.header, sizeof(WAVEHDR));
                waveOutWrite(player.hWaveOut, &player.header, sizeof(WAVEHDR));

                audio_pos = 0;
            }
        }

        if (!turbo) { while (glfwGetTime() - t1 < 0.00000016); }    // Adjust for real gigatron speed
        keyboard_handler(window);
        S = T;
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
