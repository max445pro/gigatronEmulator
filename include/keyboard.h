#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../include/core.h"
#include "../include/glfw3.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void keyboard_handler(GLFWwindow *window);

#endif
