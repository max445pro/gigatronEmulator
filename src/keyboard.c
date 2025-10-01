#include "../include/keyboard.h"
#include "../include/core.h"
#include <stdio.h>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    IN_PORT = key;
}

void keyboard_handler(GLFWwindow *window) {
    static int turbo_toggle = 0;
    // Controller is mapped to keypad
    if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_END ) == GLFW_PRESS) IN_PORT &= 0b01111111; // A
    else IN_PORT |= 0b10000000;
    if (glfwGetKey(window, GLFW_KEY_KP_7) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS) IN_PORT &= 0b10111111; // B
    else IN_PORT |= 0b01000000;
    if (glfwGetKey(window, GLFW_KEY_KP_3) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) IN_PORT &= 0b11011111; // Select
    else IN_PORT |= 0b00100000;
    if (glfwGetKey(window, GLFW_KEY_KP_9) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) IN_PORT &= 0b11101111; // Start
    else IN_PORT |= 0b00010000;
    if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_UP  ) == GLFW_PRESS) IN_PORT &= 0b11110111; // Up
    else IN_PORT |= 0b00001000;
    if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) IN_PORT &= 0b11111011; // Down
    else IN_PORT |= 0b00000100;
    if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) IN_PORT &= 0b11111101; // Left
    else IN_PORT |= 0b00000010;
    if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) IN_PORT &= 0b11111110; // Right
    else IN_PORT |= 0b00000001;
    // Turbo mode
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
        turbo_toggle = 1;
    } else if (turbo_toggle) {
        turbo ^= 1;
        turbo_toggle = 0;
    }
}
