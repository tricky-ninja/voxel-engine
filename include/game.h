#pragma once
#include "Renderer.h"
#include "GLFW/glfw3.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void init(GLFWwindow *window);

void update(float deltaTime);

void close();